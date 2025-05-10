#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"

struct Vertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
};


class D3D12Geometry
{
public:

	void UploadBuffer(ComPointer<ID3D12Resource2>& buffer, void* data, uint32_t dataSize);
	void Upload();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();
	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);

public:
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPointer<ID3D12Resource2> vertexBuffer = nullptr;
};

