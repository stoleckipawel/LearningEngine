#pragma once

#include "RHI.h"

using Microsoft::WRL::ComPtr;

// RootSignature manages the creation and access to the D3D12 root signature
class RootSignature
{
public:
    RootSignature();
    ~RootSignature();
    RootSignature(const RootSignature&) = delete;
    RootSignature& operator=(const RootSignature&) = delete;

    ComPtr<ID3D12RootSignature> Get() { return m_rootSignature; }
    ID3D12RootSignature* GetRaw() const { return m_rootSignature.Get(); }
private:
    void Create();
    ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
};