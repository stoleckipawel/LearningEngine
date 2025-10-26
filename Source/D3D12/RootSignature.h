#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "ShaderCompiler.h"

class RootSignature
{
public:
	void Create();
public:
	ComPointer<ID3D12RootSignature> rootSignature = nullptr;
};