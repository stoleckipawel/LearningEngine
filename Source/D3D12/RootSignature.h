#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "ShaderCompiler.h"

class RootSignature
{
public:
	void Create();
	ComPointer<ID3D12RootSignature> Get() { return m_rootSignature; }
private:
	ComPointer<ID3D12RootSignature> m_rootSignature = nullptr;
};