#include "D3D12ShaderCompiler.h"
#include "d3dcompiler.h"


D3D12ShaderCompiler::D3D12ShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint)
{
	ID3DBlob* blob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		name,
		nullptr,
		nullptr,
		entryPoint.c_str(),
		model.c_str(),
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&blob,
		&errorBlob);

	if (FAILED(hr)) {
		std::string message = "Shader Failed to Compile: " + std::string((char*)errorBlob->GetBufferPointer());
		LogError(message);
		return;
	}

	// Fill out a shader bytecode structure
	shaderBytecode.BytecodeLength = blob->GetBufferSize();
	shaderBytecode.pShaderBytecode = blob->GetBufferPointer();
}
