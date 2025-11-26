#include "ShaderCompiler.h"
#include "d3dcompiler.h"

// Compiles a shader from file and stores the bytecode
ShaderCompiler::ShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint)
{
	ID3DBlob* blob = nullptr;
	ID3DBlob* errorBlob = nullptr;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	// Compile the HLSL shader from file
	HRESULT hr = D3DCompileFromFile(
		name,                // Path to HLSL file
		nullptr,             // Optional macros
		nullptr,             // Optional include handler
		entryPoint.c_str(),  // Entry point function
		model.c_str(),       // Shader model (e.g., "vs_5_0")
		compileFlags,        // Compilation flags
		0,                   // Effect flags
		&blob,               // Compiled bytecode
		&errorBlob           // Error messages
	);

	if (FAILED(hr)) {
		// Output error message if compilation fails
		std::string message = "Shader Failed to Compile: ";
		if (errorBlob)
			message += std::string((char*)errorBlob->GetBufferPointer());
		LogError(message, ELogType::Fatal);
		return;
	}

	// Store compiled shader bytecode
	m_shaderBytecode.BytecodeLength = blob->GetBufferSize();
	m_shaderBytecode.pShaderBytecode = blob->GetBufferPointer();
}

ShaderCompiler::~ShaderCompiler()
{
    Release();
}

void ShaderCompiler::Release()
{
    m_shaderBytecode.BytecodeLength = 0;
    m_shaderBytecode.pShaderBytecode = nullptr;
}
