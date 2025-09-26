#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "ShaderCompiler.h"

class FRootSignature
{
public:
	bool Create();
public:
	ComPointer<ID3D12RootSignature> rootSignature = nullptr;
};