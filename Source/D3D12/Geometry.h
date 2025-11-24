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
	~Geometry();
	void UploadVertexBuffer();
	void UploadIndexBuffer();
	void Upload();
	void Release();
	void Set();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();
	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);
public:
	ComPointer<ID3D12Resource2> VertexBuffer = nullptr;
	ComPointer<ID3D12Resource2> IndexBuffer = nullptr;
private:
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};

};

