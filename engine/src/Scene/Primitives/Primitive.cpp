#include "PCH.h"
#include "Primitive.h"
#include "D3D12UploadBuffer.h"
#include "Log.h"

Primitive::Primitive(const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale) noexcept :
    m_translation(translation), m_rotationEuler(rotation), m_scale(scale)
{
	m_bWorldDirty = true;
}

// -- Transform setters/getters -------------------------------------------------
void Primitive::SetTranslation(const DirectX::XMFLOAT3& t) noexcept
{
	m_translation = t;
	InvalidateWorldCache();
}

DirectX::XMFLOAT3 Primitive::GetTranslation() const noexcept
{
	return m_translation;
}

void Primitive::SetRotationEuler(const DirectX::XMFLOAT3& r) noexcept
{
	m_rotationEuler = r;
	InvalidateWorldCache();
}

DirectX::XMFLOAT3 Primitive::GetRotationEuler() const noexcept
{
	return m_rotationEuler;
}

void Primitive::SetScale(const DirectX::XMFLOAT3& s) noexcept
{
	m_scale = s;
	InvalidateWorldCache();
}

DirectX::XMFLOAT3 Primitive::GetScale() const noexcept
{
	return m_scale;
}

// Lazy rebuild of the cached world matrix. Mutable members are used so const
// accessors (GetWorldMatrix) can rebuild transparently and remain logically const.
void Primitive::RebuildWorldIfNeeded() const noexcept
{
	if (!m_bWorldDirty)
		return;

	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotationEuler.x, m_rotationEuler.y, m_rotationEuler.z);
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	DirectX::XMMATRIX world = scale * rotation * translation;
	DirectX::XMStoreFloat4x4(&m_worldMatrixCache, world);
	m_bWorldDirty = false;
}

DirectX::XMMATRIX Primitive::GetWorldMatrix() const noexcept
{
	RebuildWorldIfNeeded();
	return DirectX::XMLoadFloat4x4(&m_worldMatrixCache);
}

DirectX::XMMATRIX Primitive::GetWorldInverseTransposeMatrix() const noexcept
{
	RebuildWorldIfNeeded();
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&m_worldMatrixCache);
	DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(nullptr, world);
	return DirectX::XMMatrixTranspose(invWorld);
}

PerObjectVSConstantBufferData Primitive::GetPerObjectVSConstants() const noexcept
{
	PerObjectVSConstantBufferData data = {};

	const DirectX::XMMATRIX world = GetWorldMatrix();
	DirectX::XMStoreFloat4x4(&data.WorldMTX, world);

	const DirectX::XMMATRIX worldInvTranspose = GetWorldInverseTransposeMatrix();
	DirectX::XMStoreFloat3x4(&data.WorldInvTransposeMTX, worldInvTranspose);

	return data;
}

DirectX::XMFLOAT3X3 Primitive::GetWorldRotationMatrix3x3() const noexcept
{
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotationEuler.x, m_rotationEuler.y, m_rotationEuler.z);
	DirectX::XMFLOAT3X3 rot3x3;
	DirectX::XMStoreFloat3x3(&rot3x3, rotation);
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

void Primitive::Bind(ID3D12GraphicsCommandList* commandList) const noexcept
{
	if (commandList == nullptr)
	{
		LOG_ERROR("Invalid command list");
		return;
	}

	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetIndexBuffer(&m_indexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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