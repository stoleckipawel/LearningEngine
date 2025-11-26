#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "ShaderCompiler.h"

// RootSignature manages the creation and access to the D3D12 root signature
class RootSignature
{
public:
	// Creates the root signature for the graphics pipeline
	void Create();

	// Returns the COM pointer to the root signature
	ComPointer<ID3D12RootSignature> Get() { return m_rootSignature; }

private:
	ComPointer<ID3D12RootSignature> m_rootSignature = nullptr; // Root signature COM pointer
};