#pragma once

// ShaderCompiler compiles HLSL shaders and provides access to compiled bytecode for Direct3D 12.
class ShaderCompiler
{
public:
	ShaderCompiler(const std::filesystem::path& fileName, const std::string& model, const std::string& entryPoint);
	~ShaderCompiler();

	ShaderCompiler(const ShaderCompiler&) = delete;
	ShaderCompiler& operator=(const ShaderCompiler&) = delete;

	inline const void* GetBuffer() const { return m_shaderBytecode.pShaderBytecode; }
	inline size_t GetSize() const { return m_shaderBytecode.BytecodeLength; }

private:
	void Reset();
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

