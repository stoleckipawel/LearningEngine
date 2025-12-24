#pragma once

// DxcShaderCompiler compiles HLSL via DXC and exposes the resulting bytecode.
class DxcShaderCompiler
{
public:
	DxcShaderCompiler(const std::filesystem::path& fileName, const std::string& model, const std::string& entryPoint);
	~DxcShaderCompiler();

	DxcShaderCompiler(const DxcShaderCompiler&) = delete;
	DxcShaderCompiler& operator=(const DxcShaderCompiler&) = delete;

	inline const void* GetBuffer() const { return m_shaderBytecode.pShaderBytecode; }
	inline size_t GetSize() const { return m_shaderBytecode.BytecodeLength; }

private:
	void ResolveAndValidatePath(const std::filesystem::path& fileName);
	void CreateDXCInterfaces();
	void LogDXCArguments();
	void LogDXCVersion();
	void DumpShaderDebugInfo();
	void LoadShaderSource();
	void CompileShader(const std::string& model, const std::string& entryPoint);
	void HandleCompileResult();

	D3D12_SHADER_BYTECODE m_shaderBytecode{};
	std::filesystem::path m_resolvedPath;
	ComPtr<IDxcCompiler3> m_dxcCompiler;
	ComPtr<IDxcUtils> m_dxcUtils;
	ComPtr<IDxcIncludeHandler> m_includeHandler;
	std::vector<LPCWSTR> m_compileArgs;
	ComPtr<IDxcBlobEncoding> m_sourceBlob;
	DxcBuffer m_sourceBuffer{};
	ComPtr<IDxcResult> m_compileResult;
};

