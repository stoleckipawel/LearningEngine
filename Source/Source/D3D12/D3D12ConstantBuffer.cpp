#include "D3D12ConstantBuffer.h"

void D3D12ConstantBuffer::Create()
{
	//Initialize constant Buffer Data members
	ZeroMemory(&constantBufferData, sizeof(ConstantBufferData));
	constantBufferData.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// Calculate the aligned size (256-byte alignment)
	constantBufferSize = (sizeof(ConstantBufferData) + 255) & ~255;

	D3D12UploadBuffer::Upload(constantBufferResource, &constantBufferData, constantBufferSize);
}

void D3D12ConstantBuffer::Update(UINT frameIndex)
{
	constantBufferData.color.x = 0.5f + 0.5f * sinf(frameIndex);
	constantBufferData.color.y = 0.5f + 0.5f * sinf(frameIndex + 2.0f);
	constantBufferData.color.z = 0.5f + 0.5f * sinf(frameIndex + 4.0f);
	memcpy(constantBufferResource, &constantBufferData, sizeof(ConstantBufferData));
}

void D3D12ConstantBuffer::CreateConstantBufferView(D3D12DescriptorHeap& descriptorHeap)
{
	constantBufferViewDesc.BufferLocation = constantBufferResource->GetGPUVirtualAddress();
	constantBufferViewDesc.SizeInBytes = constantBufferSize;
	D3D12Context::Get().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorHeap.heap->GetCPUDescriptorHandleForHeapStart());
}

void D3D12ConstantBuffer::Release()
{

}

