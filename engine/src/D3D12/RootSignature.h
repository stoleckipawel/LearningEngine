#pragma once

#include "RHI.h"

// RootSignature manages the creation and access to the D3D12 root signature
class RootSignature
{
public:
	// Constructs and creates the root signature for the graphics pipeline
	RootSignature();

	// Destructor releases the root signature
	~RootSignature();

	// Deleted copy constructor and assignment operator to enforce unique ownership
	RootSignature(const RootSignature&) = delete;
	RootSignature& operator=(const RootSignature&) = delete;

	// Returns the COM pointer to the root signature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> Get() { return m_rootSignature; }
private:
	void Create();
	void Reset();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr; // Root signature COM pointer
};