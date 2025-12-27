#include "PCH.h"
#include "Primitive.h"
#include "D3D12UploadBuffer.h"
#include "D3D12SwapChain.h"
#include "Camera.h"

Primitive::Primitive(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    m_translation(translation), m_rotationEuler(rotation), m_scale(scale)
{
	m_bWorldDirty = true;
}

// -- Transform setters/getters -------------------------------------------------
void Primitive::SetTranslation(const XMFLOAT3& t) noexcept
{
	m_translation = t;
	InvalidateWorldCache();
}

XMFLOAT3 Primitive::GetTranslation() const noexcept
{
	return m_translation;
}

void Primitive::SetRotationEuler(const XMFLOAT3& r) noexcept
{
	m_rotationEuler = r;
	InvalidateWorldCache();
}

XMFLOAT3 Primitive::GetRotationEuler() const noexcept
{
	return m_rotationEuler;
}

void Primitive::SetScale(const XMFLOAT3& s) noexcept
{
	m_scale = s;
	InvalidateWorldCache();
}

XMFLOAT3 Primitive::GetScale() const noexcept
{
	return m_scale;
}

// Lazy rebuild of the cached world matrix. Mutable members are used so const
// accessors (GetWorldMatrix) can rebuild transparently and remain logically const.
void Primitive::RebuildWorldIfNeeded() const noexcept
{
	if (!m_bWorldDirty)
		return;

	XMMATRIX translation = XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_rotationEuler.x, m_rotationEuler.y, m_rotationEuler.z);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX world = scale * rotation * translation;
	XMStoreFloat4x4(&m_worldMatrixCache, world);
	m_bWorldDirty = false;
}

XMMATRIX Primitive::GetWorldMatrix() const noexcept
{
	RebuildWorldIfNeeded();
	return XMLoadFloat4x4(&m_worldMatrixCache);
}

XMMATRIX Primitive::GetWorldInverseTransposeMatrix() const noexcept
{
	RebuildWorldIfNeeded();
	XMMATRIX world = XMLoadFloat4x4(&m_worldMatrixCache);
	XMMATRIX invWorld = XMMatrixInverse(nullptr, world);
	return XMMatrixTranspose(invWorld);
}

DirectX::XMFLOAT3X3 Primitive::GetWorldRotationMatrix3x3() const noexcept
{
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_rotationEuler.x, m_rotationEuler.y, m_rotationEuler.z);
	DirectX::XMFLOAT3X3 rot3x3;
	XMStoreFloat3x3(&rot3x3, rotation);
	return rot3x3;
}

void Primitive::UploadVertexBuffer()
{
	std::vector<Vertex> vertexList;
	GenerateVertices(vertexList);
	const UINT vertsDataSize = static_cast<UINT>(sizeof(Vertex) * vertexList.size());
	VertexBuffer = D3D12UploadBuffer::Upload(vertexList.data(), vertsDataSize);
	m_vertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = vertsDataSize;
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void Primitive::UploadIndexBuffer()
{
	std::vector<DWORD> indexList;
	GenerateIndices(indexList);
	m_indexCount = static_cast<UINT>(indexList.size());
	UINT indexDataSize = static_cast<UINT>(sizeof(DWORD) * indexList.size());
	IndexBuffer = D3D12UploadBuffer::Upload(indexList.data(), indexDataSize);
	m_indexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = indexDataSize;
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Primitive::Upload()
{
	UploadIndexBuffer();
	UploadVertexBuffer();
}

void Primitive::Set()
{
	GD3D12Rhi.GetCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	GD3D12Rhi.GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
	GD3D12Rhi.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

std::span<const D3D12_INPUT_ELEMENT_DESC> Primitive::GetStaticVertexLayout() noexcept
{
	static const D3D12_INPUT_ELEMENT_DESC s_layout[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	return std::span<const D3D12_INPUT_ELEMENT_DESC>(s_layout);
}

std::vector<D3D12_INPUT_ELEMENT_DESC> Primitive::GetVertexLayout() const
{
	auto layout = GetStaticVertexLayout();
	return std::vector<D3D12_INPUT_ELEMENT_DESC>(layout.begin(), layout.end());
}

D3D12_RESOURCE_DESC Primitive::CreateVertexBufferDesc(uint32_t VertexCount) const
{
	D3D12_RESOURCE_DESC vertexResourceDesc = {};
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	vertexResourceDesc.Width = VertexCount;
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexResourceDesc.SampleDesc.Count = 1;
	vertexResourceDesc.SampleDesc.Quality = 0;
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	return vertexResourceDesc;
}
