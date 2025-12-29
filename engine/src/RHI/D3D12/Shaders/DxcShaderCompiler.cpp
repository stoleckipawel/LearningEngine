#include "PCH.h"
#include "DxcShaderCompiler.h"
#include "DxcContext.h"
#include "AssetPathResolver.h"

namespace
{
	// Walks up from a resolved shader path to find the "shaders" directory.
	std::filesystem::path FindShaderIncludeRoot(const std::filesystem::path& resolvedPath)
	{
		std::filesystem::path dir = resolvedPath.parent_path();
		while (!dir.empty())
		{
			if (dir.filename() == "shaders")
				return dir;
			dir = dir.parent_path();
		}
		return resolvedPath.parent_path();
	}

	// Converts a narrow string to wide string.
	std::wstring ToWide(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}

	// Converts a path to wide string for DXC.
	std::wstring ToWide(const std::filesystem::path& path)
	{
		return path.wstring();
	}
}  // namespace

ShaderCompileResult
DxcShaderCompiler::CompileFromAsset(const std::filesystem::path& sourcePath, ShaderStage stage, const std::string& entryPoint)
{
	std::filesystem::path resolvedPath = ResolveAssetPath(sourcePath, AssetType::Shader);
	if (resolvedPath.empty() || !std::filesystem::exists(resolvedPath))
	{
		return ShaderCompileResult::Failure("Shader file not found: " + sourcePath.string());
	}

	ShaderCompileOptions options;
	options.SourcePath = resolvedPath;
	options.IncludeDir = FindShaderIncludeRoot(resolvedPath);
	options.EntryPoint = entryPoint;
	options.Stage = stage;

#if defined(ENGINE_SHADERS_DEBUG)
	options.EnableDebugInfo = true;
#endif

#if defined(ENGINE_SHADERS_OPTIMIZED)
	options.EnableOptimizations = true;
#else
	options.EnableOptimizations = false;
#endif

	LOG_INFO("Compiling shader: " + resolvedPath.string());
	return Compile(options);
}

ShaderCompileResult DxcShaderCompiler::Compile(const ShaderCompileOptions& options)
{
	DxcContext& ctx = GetDxcContext();
	if (!ctx.IsValid())
	{
		return ShaderCompileResult::Failure("DXC context is not initialized");
	}

	// Load source file
	ComPtr<IDxcBlobEncoding> sourceBlob;
	HRESULT hr = ctx.GetUtils()->LoadFile(options.SourcePath.c_str(), nullptr, sourceBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr) || !sourceBlob)
	{
		return ShaderCompileResult::Failure("Failed to load shader source: " + options.SourcePath.string());
	}

	DxcBuffer sourceBuffer{};
	sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
	sourceBuffer.Size = sourceBlob->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_ACP;

	// Build compile arguments - store wide strings to keep pointers valid
	std::wstring wSourcePath = ToWide(options.SourcePath);
	std::wstring wEntryPoint = ToWide(options.EntryPoint);
	std::wstring wTargetProfile = ToWide(options.BuildTargetProfile());
	std::vector<std::wstring> wIncludeDirs;
	std::vector<std::wstring> wDefines;
	std::vector<LPCWSTR> args;

	BuildCompileArguments(options, wSourcePath, wEntryPoint, wTargetProfile, wIncludeDirs, wDefines, args);

	// Create include handler and compile
	ComPtr<IDxcIncludeHandler> includeHandler = ctx.CreateIncludeHandler();

	ComPtr<IDxcResult> result;
	hr = ctx.GetCompiler()->Compile(
	    &sourceBuffer,
	    args.data(),
	    static_cast<UINT>(args.size()),
	    includeHandler.Get(),
	    IID_PPV_ARGS(result.ReleaseAndGetAddressOf()));

	if (FAILED(hr) || !result)
	{
		return ShaderCompileResult::Failure("DXC Compile() call failed");
	}

	// Check for errors
	std::string errorMsg = ExtractErrorMessage(result.Get());

	HRESULT status;
	result->GetStatus(&status);
	if (FAILED(status))
	{
		if (errorMsg.empty())
			errorMsg = "Compilation failed with no error message";
		LOG_FATAL("Shader compilation failed: " + errorMsg);
		return ShaderCompileResult::Failure(std::move(errorMsg));
	}

	// Log warnings if present
	if (!errorMsg.empty())
	{
		LOG_WARNING("Shader warnings: " + errorMsg);
	}

	// Extract bytecode
	std::vector<uint8_t> bytecode = ExtractBytecode(result.Get());
	if (bytecode.empty())
	{
		return ShaderCompileResult::Failure("Failed to extract shader bytecode");
	}

	// Save debug symbols
	SaveShaderSymbols(result.Get(), options.SourcePath);

	LOG_INFO("Shader compiled successfully: " + options.SourcePath.filename().string());
	return ShaderCompileResult::Success(std::move(bytecode));
}

void DxcShaderCompiler::BuildCompileArguments(
    const ShaderCompileOptions& options,
    const std::wstring& wSourcePath,
    const std::wstring& wEntryPoint,
    const std::wstring& wTargetProfile,
    std::vector<std::wstring>& wIncludeDirs,
    std::vector<std::wstring>& wDefines,
    std::vector<LPCWSTR>& outArgs)
{
	outArgs.clear();
	outArgs.reserve(32);

	// Source file (for error messages and PIX)
	outArgs.push_back(wSourcePath.c_str());

	// Entry point
	outArgs.push_back(L"-E");
	outArgs.push_back(wEntryPoint.c_str());

	// Target profile
	outArgs.push_back(L"-T");
	outArgs.push_back(wTargetProfile.c_str());

	// HLSL version
	outArgs.push_back(L"-HV");
	outArgs.push_back(L"2021");

	// Include directories
	wIncludeDirs.clear();
	wIncludeDirs.push_back(ToWide(options.IncludeDir));
	for (const auto& dir : options.AdditionalIncludeDirs)
	{
		wIncludeDirs.push_back(ToWide(dir));
	}
	for (const auto& dir : wIncludeDirs)
	{
		outArgs.push_back(L"-I");
		outArgs.push_back(dir.c_str());
	}

	// Preprocessor defines
	wDefines.clear();
	for (const auto& def : options.Defines)
	{
		wDefines.push_back(ToWide(def));
	}
	for (const auto& def : wDefines)
	{
		outArgs.push_back(L"-D");
		outArgs.push_back(def.c_str());
	}

	// Strictness and resource binding
	outArgs.push_back(DXC_ARG_ENABLE_STRICTNESS);
	outArgs.push_back(DXC_ARG_ALL_RESOURCES_BOUND);

	// Warnings
	if (options.TreatWarningsAsErrors)
	{
		outArgs.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
	}

	// Stripping
	if (options.StripReflection)
	{
		outArgs.push_back(L"-Qstrip_reflect");
	}
	if (options.StripDebugInfo)
	{
		outArgs.push_back(L"-Qstrip_debug");
	}

	// Debug info
	if (options.EnableDebugInfo)
	{
		outArgs.push_back(DXC_ARG_DEBUG);
	}

	// Optimization level
	if (options.EnableOptimizations)
	{
		outArgs.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
	}
	else
	{
		outArgs.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
	}
}

std::vector<uint8_t> DxcShaderCompiler::ExtractBytecode(IDxcResult* result)
{
	ComPtr<IDxcBlob> shaderBlob;
	HRESULT hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.ReleaseAndGetAddressOf()), nullptr);
	if (FAILED(hr) || !shaderBlob || shaderBlob->GetBufferSize() == 0)
	{
		return {};
	}

	const uint8_t* data = static_cast<const uint8_t*>(shaderBlob->GetBufferPointer());
	return std::vector<uint8_t>(data, data + shaderBlob->GetBufferSize());
}

std::string DxcShaderCompiler::ExtractErrorMessage(IDxcResult* result)
{
	ComPtr<IDxcBlobUtf8> errorBlob;
	result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBlob.ReleaseAndGetAddressOf()), nullptr);
	if (errorBlob && errorBlob->GetStringLength() > 0)
	{
		return std::string(errorBlob->GetStringPointer(), errorBlob->GetStringLength());
	}
	return {};
}

void DxcShaderCompiler::SaveShaderSymbols(IDxcResult* result, const std::filesystem::path& sourcePath)
{
	ComPtr<IDxcBlob> pdbBlob;
	ComPtr<IDxcBlobUtf16> pdbNameBlob;
	result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(pdbBlob.ReleaseAndGetAddressOf()), pdbNameBlob.ReleaseAndGetAddressOf());

	if (!pdbBlob || !pdbNameBlob)
		return;

	// Build output path: ShaderSymbols/<shader_name>.pdb
	std::filesystem::path symbolsDir = sourcePath.parent_path() / "ShaderSymbols";
	std::error_code ec;
	std::filesystem::create_directories(symbolsDir, ec);

	std::wstring pdbName(pdbNameBlob->GetStringPointer());
	std::filesystem::path pdbFilename = std::filesystem::path(pdbName).filename();
	std::filesystem::path pdbPath = symbolsDir / pdbFilename;

	FILE* fp = nullptr;
	_wfopen_s(&fp, pdbPath.c_str(), L"wb");
	if (fp)
	{
		fwrite(pdbBlob->GetBufferPointer(), 1, pdbBlob->GetBufferSize(), fp);
		fclose(fp);
	}
}