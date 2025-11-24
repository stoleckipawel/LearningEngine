#include "ShaderCompiler.h"
#include "d3dcompiler.h"


ShaderCompiler::ShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint)
{
	ID3DBlob* blob = nullptr;
	ID3DBlob* errorBlob = nullptr;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	HRESULT hr = D3DCompileFromFile(
		name,
		nullptr,
		nullptr,
		entryPoint.c_str(),
		model.c_str(),
		compileFlags,
		0,
		&blob,
		&errorBlob);

	if (FAILED(hr)) {
		std::string message = "Shader Failed to Compile: " + std::string((char*)errorBlob->GetBufferPointer());
		LogError(message, ELogType::Fatal);
		return;
	}

	// Fill out a shader bytecode structure
	m_shaderBytecode.BytecodeLength = blob->GetBufferSize();
	m_shaderBytecode.pShaderBytecode = blob->GetBufferPointer();
}
