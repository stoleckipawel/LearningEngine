#pragma once

// ShaderCompiler compiles HLSL shaders and provides access to compiled bytecode for Direct3D 12.
class ShaderCompiler
{
public:
	// Constructs and compiles a shader from file
	ShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint);

	// Destructor releases shader bytecode
	~ShaderCompiler();

	// Deleted copy constructor and assignment operator to enforce unique ownership
	ShaderCompiler(const ShaderCompiler&) = delete;
	ShaderCompiler& operator=(const ShaderCompiler&) = delete;

	// Returns pointer to compiled shader bytecode
	inline const void* GetBuffer() const { return m_shaderBytecode.pShaderBytecode; }
	// Returns size of compiled shader bytecode
	inline size_t GetSize() const { return m_shaderBytecode.BytecodeLength; }
private:
	void Release();
	D3D12_SHADER_BYTECODE m_shaderBytecode{};
};

