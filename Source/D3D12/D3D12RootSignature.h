#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"
#include "D3D12ShaderCompiler.h"

class D3D12RootSignature
{
public:
	bool Create();
public:
	ComPointer<ID3D12RootSignature> rootSignature = nullptr;
};

