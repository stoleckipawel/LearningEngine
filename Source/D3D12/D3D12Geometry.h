#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"
#include <DirectXMath.h>

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};


class D3D12Geometry
{
public:

	void UploadBuffer(ComPointer<ID3D12Resource2>& buffer, void* data, uint32_t dataSize);
	void UploadVertexBuffer();
	void UploadIndexBuffer();
	void Upload();
	void Set(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();
	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);

public:
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPointer<ID3D12Resource2> vertexBuffer = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	ComPointer<ID3D12Resource2> indexBuffer = nullptr;
};

