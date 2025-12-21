#include "PCH.h"
#include "ShaderCompiler.h"
#include "AssetPathResolver.h"

// Compiles a shader from file and stores the bytecode using DXC
// fileName: name of HLSL file
// model: shader model string (e.g., "vs_6_0", "ps_6_0")
// entryPoint: entry function name (usually "main")
ShaderCompiler::ShaderCompiler(const std::filesystem::path& fileName, const std::string& model, const std::string& entryPoint)
{
    // Orchestrates shader compilation steps
    // 1. Resolve and validate shader file path
    // 2. Create DXC compiler and utility interfaces
    // 3. Prepare DXC compile arguments
    // 4. Load shader source file into DXC blob
    // 5. Compile shader
    // 6. Handle compilation result and output

    ResolveAndValidatePath(fileName);
    CreateDXCInterfaces();
	LoadShaderSource();
    CompileShader(model, entryPoint);
    HandleCompileResult();
}

// Releases shader compiler resources
ShaderCompiler::~ShaderCompiler()
{
    m_shaderBytecode.BytecodeLength = 0;
    m_shaderBytecode.pShaderBytecode = nullptr;
}

// Resolves the asset path for the shader file and checks if it exists.
void ShaderCompiler::ResolveAndValidatePath(const std::filesystem::path& fileName)
{
    // Resolve asset path for shader file and validate existence
    m_resolvedPath = ResolveAssetPath(fileName, AssetType::Shader);
    if (!std::filesystem::exists(m_resolvedPath)) {
        LogMessage("Shader file does not exist: " + m_resolvedPath.string(), ELogType::Fatal);
    }
}

// Creates DXC compiler, utility, and include handler interfaces.
void ShaderCompiler::CreateDXCInterfaces()
{
    // Create DXC compiler interface
    HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_dxcCompiler.ReleaseAndGetAddressOf()));
    if (FAILED(hr)) {
        LogMessage("Failed to create DXC compiler", ELogType::Fatal);
        return;
    }
    // Create DXC utility interface
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_dxcUtils.ReleaseAndGetAddressOf()));
    if (FAILED(hr)) {
        LogMessage("Failed to create DXC utils", ELogType::Fatal);
        return;
    }
    // Create default include handler for #include directives
    hr = m_dxcUtils->CreateDefaultIncludeHandler(m_includeHandler.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        LogMessage("Failed to create DXC include handler", ELogType::Fatal);
        return;
    }
}

// Handles DXC compilation result: logs errors, checks status, saves binary and PDB outputs.
void ShaderCompiler::HandleCompileResult()
{
    // Print errors and warnings if present from DXC
    ComPtr<IDxcBlobUtf8> errorBlob = nullptr;
    m_compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBlob.ReleaseAndGetAddressOf()), nullptr);
    if (errorBlob != nullptr && errorBlob->GetStringLength() != 0) {
        LogMessage(std::string("DXC Warnings/Errors: ") + std::string(errorBlob->GetStringPointer(), errorBlob->GetStringLength()), ELogType::Fatal);
    }

    // Check compilation status and abort if failed
    HRESULT hrStatus;
    m_compileResult->GetStatus(&hrStatus);
    if (FAILED(hrStatus)) {
        LogMessage("DXC Compilation Failed", ELogType::Fatal);
        DumpShaderDebugInfo();
    }

    // Save compiled shader binary to disk and store bytecode for engine use
    ComPtr<IDxcBlob> shaderBlob = nullptr;
    ComPtr<IDxcBlobUtf16> shaderNameBlob = nullptr;
    m_compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.ReleaseAndGetAddressOf()), shaderNameBlob.ReleaseAndGetAddressOf());
    if (shaderBlob != nullptr && shaderNameBlob != nullptr) {
        FILE* fp = nullptr;
        _wfopen_s(&fp, shaderNameBlob->GetStringPointer(), L"wb");
        fwrite(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 1, fp);
        fclose(fp);
        // Convert wide string to UTF-8 for logging
        std::wstring ws(shaderNameBlob->GetStringPointer());
        std::string filename(ws.begin(), ws.end());
        LogMessage("Shader binary saved: " + filename, ELogType::Info);
    }
    m_shaderBytecode.BytecodeLength = shaderBlob->GetBufferSize();
    m_shaderBytecode.pShaderBytecode = shaderBlob->GetBufferPointer();

    // Save PDB (debug info) to disk for debugging and PIX integration
    ComPtr<IDxcBlob> pdbBlob = nullptr;
    ComPtr<IDxcBlobUtf16> pdbNameBlob = nullptr;
    m_compileResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(pdbBlob.ReleaseAndGetAddressOf()), pdbNameBlob.ReleaseAndGetAddressOf());
    if (pdbBlob != nullptr && pdbNameBlob != nullptr) {
        FILE* fp = nullptr;
        _wfopen_s(&fp, pdbNameBlob->GetStringPointer(), L"wb");
        fwrite(pdbBlob->GetBufferPointer(), pdbBlob->GetBufferSize(), 1, fp);
        fclose(fp);
    }
}

// Log DXC Shader Debug Info
void ShaderCompiler::DumpShaderDebugInfo()
{
    LogDXCVersion();
    LogDXCArguments();
}

// Loads the shader source file into a DXC blob for compilation.
void ShaderCompiler::LoadShaderSource()
{
    // Load shader source file into DXC blob
    HRESULT hr = m_dxcUtils->LoadFile(m_resolvedPath.c_str(), nullptr, m_sourceBlob.ReleaseAndGetAddressOf());
    if (FAILED(hr) || !m_sourceBlob) {
        LogMessage("Failed to load shader source file: " + m_resolvedPath.string(), ELogType::Fatal);
        return;
    }
    // Fill DXC buffer struct with source blob data
    m_sourceBuffer.Ptr = m_sourceBlob->GetBufferPointer();
    m_sourceBuffer.Size = m_sourceBlob->GetBufferSize();
    m_sourceBuffer.Encoding = DXC_CP_ACP; // Assume ANSI text or BOM specifies encoding
}

// Prepares DXC compile arguments and invokes DXC to compile the shader.
void ShaderCompiler::CompileShader(const std::string& model, const std::string& entryPoint)
{
    // Prepare DXC compile arguments for entry point, target profile, resource binding, warnings, reflection, debug info, and optimization
    std::wstring wEntry(entryPoint.begin(), entryPoint.end());
    std::wstring wModel(model.begin(), model.end());
    // Build LPCWSTR argument array as in Microsoft DXC samples
        LPCWSTR pszArgs[] = {
        m_resolvedPath.c_str(), // Shader source file path (for error reporting and PIX source view)
        L"-E",                 // Entry point flag
        wEntry.c_str(),         // Entry point function name
        L"-T",                 // Target profile flag
        wModel.c_str(),         // Target profile (e.g., "vs_6_0", "ps_6_0")
        DXC_ARG_ALL_RESOURCES_BOUND, // Assume all resources are bound
        DXC_ARG_WARNINGS_ARE_ERRORS, // Treat warnings as errors
        L"-Qstrip_reflect",    // Strip reflection data into a separate blob
        L"-Qstrip_debug",      // Strip debug info from output
    
    #if defined(ENGINE_SHADERS_DEBUG)
        DXC_ARG_DEBUG,          // Enable debug info (full format)
    #endif    

    #if defined(ENGINE_SHADERS_OPTIMIZED)    
        DXC_ARG_OPTIMIZATION_LEVEL3, // Enable full optimizations for release    
    #else
        DXC_ARG_SKIP_OPTIMIZATIONS, // Skip optimizations for easier debugging
    #endif
        };

    // Store pointers for debug printing
    m_compileArgs.assign(std::begin(pszArgs), std::end(pszArgs));

    // Compile shader using DXC
    HRESULT hr = m_dxcCompiler->Compile(
        &m_sourceBuffer,                  // Source buffer
        pszArgs,                          // Compile arguments
        static_cast<UINT>(std::size(pszArgs)), // Number of arguments
        m_includeHandler.Get(),                 // #include handler
        IID_PPV_ARGS(m_compileResult.ReleaseAndGetAddressOf())    // Compiler output
    );

    if (FAILED(hr) || !m_compileResult) {
        LogMessage("DXC failed to compile shader", ELogType::Fatal);
        return;
    }
}

// Logs the DXC compile arguments for debugging purposes.
void ShaderCompiler::LogDXCArguments()
{
    // Only log in debug builds
#if defined(_DEBUG)
    constexpr size_t kMaxArgsToLog = 32;
    std::string log;
    log.reserve(256);
    log += "DXC Compile Arguments:\n";
    size_t count = std::min(m_compileArgs.size(), kMaxArgsToLog);
    for (size_t i = 0; i < count; ++i) {
        // Convert wide string to UTF-8 for logging
        const wchar_t* arg = m_compileArgs[i];
        std::wstring ws(arg);
        std::string utf8(ws.begin(), ws.end());
        log += "  [" + std::to_string(i) + "] " + utf8 + "\n";
    }
    if (m_compileArgs.size() > kMaxArgsToLog)
        log += "  ... (truncated)\n";
    LogMessage(log, ELogType::Info);
#endif
}


// Logs DXC version for debugging
void ShaderCompiler::LogDXCVersion()
{
#if defined(_DEBUG)
    ComPtr<IDxcVersionInfo> versionInfo = nullptr;
    if (SUCCEEDED(m_dxcCompiler->QueryInterface(__uuidof(IDxcVersionInfo), reinterpret_cast<void**>(versionInfo.ReleaseAndGetAddressOf())))) {
        UINT major = 0, minor = 0;
        versionInfo->GetVersion(&major, &minor);
        LogMessage("DXC Version: " + std::to_string(major) + "." + std::to_string(minor), ELogType::Info);
    } 
	else 
	{
        LogMessage("DXC Version: <unavailable>", ELogType::Info);
    }
#endif
}