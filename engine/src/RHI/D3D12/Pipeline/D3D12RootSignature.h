#pragma once

#include "D3D12Rhi.h"

using Microsoft::WRL::ComPtr;

// RootSignature manages the creation and access to the D3D12 root signature
class D3D12RootSignature
{
  public:
	D3D12RootSignature();
	~D3D12RootSignature();
	D3D12RootSignature(const D3D12RootSignature&) = delete;
	D3D12RootSignature& operator=(const D3D12RootSignature&) = delete;

	ComPtr<ID3D12RootSignature> Get()
	{
		return m_rootSignature;
	}
	ID3D12RootSignature* GetRaw() const
	{
		return m_rootSignature.Get();
	}

  private:
	void Create();
	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
};