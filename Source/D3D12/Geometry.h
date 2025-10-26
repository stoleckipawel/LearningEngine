#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include <DirectXMath.h>


struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT4 color;
};


class Geometry
{
public:
	void UploadVertexBuffer();
	void UploadIndexBuffer();
	void Upload();
	void Release();
	void Set();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();
	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);

public:
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	ComPointer<ID3D12Resource2> vertexBuffer = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	ComPointer<ID3D12Resource2> indexBuffer = nullptr;
};

