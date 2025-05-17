#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12UploadBuffer.h"

struct ConstantBufferData
{
	XMFLOAT4 color;
};

class D3D12ConstantBuffer
{
public:
	void Create();
	void Update();
	void Set();
	void Release();
	void CreateConstantBufferView(D3D12DescriptorHeap& descriptorHeap);
public:
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
	ComPointer<ID3D12Resource2> constantBufferResource = nullptr;
	ConstantBufferData constantBufferData;
	UINT constantBufferSize = 0;


};


