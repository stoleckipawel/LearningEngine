#pragma once

#include <cstring>
#include <cstdlib>
#include <string_view>
#include <filesystem>
#include <fstream>
#include "../Vendor/Windows/WinInclude.h"
#include <iostream>

class FShaderCompiler
{
public:
    FShaderCompiler() = default;
    FShaderCompiler(LPCWSTR name, const std::string& model, const std::string& entryPoint);
	inline const void* GetBuffer() const { return shaderBytecode.pShaderBytecode; }
	inline size_t GetSize() const { return shaderBytecode.BytecodeLength; }
private:
	D3D12_SHADER_BYTECODE shaderBytecode;
};

