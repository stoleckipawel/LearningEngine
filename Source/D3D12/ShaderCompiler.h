#pragma once

#include <cstring>
#include <cstdlib>
#include <string_view>
#include <filesystem>
#include <fstream>
#include "../Vendor/Windows/WinInclude.h"
#include <iostream>

class ShaderCompiler
{
public:
    ShaderCompiler() = default;
    ShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint);
	inline const void* GetBuffer() const { return m_shaderBytecode.pShaderBytecode; }
	inline size_t GetSize() const { return m_shaderBytecode.BytecodeLength; }
private:
	D3D12_SHADER_BYTECODE m_shaderBytecode;
};

