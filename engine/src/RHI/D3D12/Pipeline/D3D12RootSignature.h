// ============================================================================
// D3D12RootSignature.h
// ----------------------------------------------------------------------------
// Manages creation and access to the D3D12 root signature.
//
// USAGE:
//   D3D12RootSignature rootSig;
//   psoDesc.pRootSignature = rootSig.GetRaw();
//
// DESIGN:
//   - Root signature layout defined in D3D12RootBindings.h
//   - Created automatically in constructor
//   - Non-copyable to prevent signature duplication
//
// NOTES:
//   - Sync root parameter layout with D3D12RootBindings.h
//   - Sync shader register declarations with Common.hlsli
// ============================================================================

#pragma once

#include "D3D12Rhi.h"

using Microsoft::WRL::ComPtr;

class D3D12RootSignature
{
  public:
	D3D12RootSignature();
	~D3D12RootSignature();
	D3D12RootSignature(const D3D12RootSignature&) = delete;
	D3D12RootSignature& operator=(const D3D12RootSignature&) = delete;

	ComPtr<ID3D12RootSignature> Get() { return m_rootSignature; }
	ID3D12RootSignature* GetRaw() const { return m_rootSignature.Get(); }

  private:
	void Create();
	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
};