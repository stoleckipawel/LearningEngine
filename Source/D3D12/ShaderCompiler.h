#pragma once

#include <cstring>
#include <cstdlib>
#include <string_view>
#include <filesystem>
#include <fstream>
#include "../Vendor/Windows/WinInclude.h"
#include <iostream>


// ShaderCompiler compiles HLSL shaders and provides access to compiled bytecode for Direct3D 12.
class ShaderCompiler
{
public:
	ShaderCompiler() = default;

	// Compiles a shader from file
	// name: Path to the HLSL file
	// model: Shader model (e.g., "vs_5_0", "ps_5_0")
	// entryPoint: Entry point function name
	ShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint);

	// Returns pointer to compiled shader bytecode
	inline const void* GetBuffer() const { return m_shaderBytecode.pShaderBytecode; }
	// Returns size of compiled shader bytecode
	inline size_t GetSize() const { return m_shaderBytecode.BytecodeLength; }
private:
	// Holds compiled shader bytecode and its length
	D3D12_SHADER_BYTECODE m_shaderBytecode{};
};

