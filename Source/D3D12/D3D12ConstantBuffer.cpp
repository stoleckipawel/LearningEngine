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

void D3D12ConstantBuffer::CreateConstantBufferView(D3D12DescriptorHeap& descriptorHeap)
{
	constantBufferViewDesc.BufferLocation = constantBufferResource->GetGPUVirtualAddress();
	constantBufferViewDesc.SizeInBytes = constantBufferSize;
	D3D12Context::Get().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorHeap.heap->GetCPUDescriptorHandleForHeapStart());
}


void D3D12ConstantBuffer::Update()
{
/*
	// update app logic, such as moving the camera or figuring out what objects are in view
	static float rIncrement = 0.00002f;
	static float gIncrement = 0.00006f;
	static float bIncrement = 0.00009f;

	cbColorMultiplierData.colorMultiplier.x += rIncrement;
	cbColorMultiplierData.colorMultiplier.y += gIncrement;
	cbColorMultiplierData.colorMultiplier.z += bIncrement;

	if (cbColorMultiplierData.colorMultiplier.x >= 1.0 || cbColorMultiplierData.colorMultiplier.x <= 0.0)
	{
		cbColorMultiplierData.colorMultiplier.x = cbColorMultiplierData.colorMultiplier.x >= 1.0 ? 1.0 : 0.0;
		rIncrement = -rIncrement;
	}
	if (cbColorMultiplierData.colorMultiplier.y >= 1.0 || cbColorMultiplierData.colorMultiplier.y <= 0.0)
	{
		cbColorMultiplierData.colorMultiplier.y = cbColorMultiplierData.colorMultiplier.y >= 1.0 ? 1.0 : 0.0;
		gIncrement = -gIncrement;
	}
	if (cbColorMultiplierData.colorMultiplier.z >= 1.0 || cbColorMultiplierData.colorMultiplier.z <= 0.0)
	{
		cbColorMultiplierData.colorMultiplier.z = cbColorMultiplierData.colorMultiplier.z >= 1.0 ? 1.0 : 0.0;
		bIncrement = -bIncrement;
	}

	// copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbColorMultiplierGPUAddress[frameIndex], &cbColorMultiplierData, sizeof(cbColorMultiplierData));
	*/
}

void D3D12ConstantBuffer::Set()
{
	/*
	// set constant buffer descriptor heap
	ID3D12DescriptorHeap* descriptorHeaps[] = { mainDescriptorHeap[frameIndex] };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// set the root descriptor table 0 to the constant buffer descriptor heap
	commandList->SetGraphicsRootDescriptorTable(0, mainDescriptorHeap[frameIndex]->GetGPUDescriptorHandleForHeapStart());
	*/
}

void D3D12ConstantBuffer::Release()
{
	//SAFE_RELEASE(mainDescriptorHeap[i]);
	//SAFE_RELEASE(constantBufferUploadHeap[i]);
}

