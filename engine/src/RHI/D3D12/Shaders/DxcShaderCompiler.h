#pragma once

#include "EngineConfig.h"
#include <string>
#include <filesystem>
#include <vector>

using Microsoft::WRL::ComPtr;

// Compiles HLSL using DXC and provides the resulting shader bytecode
// suitable for creating GPU pipeline state objects.
class DxcShaderCompiler
{
	public:
		// Shader stages the engine supports. Keep entries explicit so callers
		// pass a clear intent when requesting compilation.
		enum class ShaderStage
		{
				Vertex,
				Pixel,
				Geometry,
				Hull,
				Domain,
				Compute,
		};

		// Returns the DXC target profile string for `stage`, for example "vs_6_0".
		// This uses the global shader model defined in `EngineConfig.h`.
		static std::string BuildShaderProfile(ShaderStage stage);

		// Compile `fileName` using `stage` and `entryPoint`. The compiled bytecode
		// is accessible via `GetBuffer()`/`GetSize()` on success.
		DxcShaderCompiler(const std::filesystem::path& fileName, ShaderStage stage, const std::string& entryPoint);

		// Releases internal references to compiled shader data.
		~DxcShaderCompiler();

		DxcShaderCompiler(const DxcShaderCompiler&) = delete;
		DxcShaderCompiler& operator=(const DxcShaderCompiler&) = delete;

		// Pointer to compiled shader bytecode and its size for pipeline creation.
		inline const void* GetBuffer() const { return m_shaderBytecode.pShaderBytecode; }
		inline size_t GetSize() const { return m_shaderBytecode.BytecodeLength; }

	private:
		// Resolve asset path and validate that the shader file exists.
		void ResolveAndValidatePath(const std::filesystem::path& fileName);

		// Initialize DXC interfaces required for compilation.
		void CreateDXCInterfaces();

		// Diagnostic helpers used when compilation fails or for debug logging.
		void LogDXCArguments();
		void LogDXCVersion();
		void DumpShaderDebugInfo();

		// Read shader source into a DXC blob and prepare the DXC buffer.
		void LoadShaderSource();

		// Perform compilation for `stage` and `entryPoint` using DXC.
		void CompileShader(ShaderStage stage, const std::string& entryPoint);

		// Inspect and store compilation outputs (object, pdb, errors).
		void HandleCompileResult();
		
		//Save Shader PDB's to ShaderSymbols folder
		void SaveShaderSymbols(IDxcBlob* pdbBlob, IDxcBlobUtf16* pdbNameBlob);

		// Compiled bytecode ready for creating pipeline state objects.
		D3D12_SHADER_BYTECODE m_shaderBytecode{};

		// Resolved on-disk path for the shader asset.
		std::filesystem::path m_resolvedPath;

		// DXC interfaces used during compilation.
		ComPtr<IDxcCompiler3> m_dxcCompiler;
		ComPtr<IDxcUtils> m_dxcUtils;
		ComPtr<IDxcIncludeHandler> m_includeHandler;

		// Arguments passed to DXC (kept for logging/debugging).
		std::vector<LPCWSTR> m_compileArgs;

		// Source blob and buffer passed to the DXC compiler.
		ComPtr<IDxcBlobEncoding> m_sourceBlob;
		DxcBuffer m_sourceBuffer{};

		// Output result from the DXC compile operation.
		ComPtr<IDxcResult> m_compileResult;
};
