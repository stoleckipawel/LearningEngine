#include "PCH.h"
#include "Primitive.h"
#include "UploadBuffer.h"
#include "SwapChain.h"
#include "Camera.h"

Primitive::Primitive(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
    : Translation(translation), Rotation(rotation), Scale(scale)
{
    for (size_t i = 0; i < EngineSettings::FramesInFlight; ++i)
    {
        VertexConstantBuffer[i] = std::make_unique<ConstantBuffer<FVertexConstantBufferData>>();
        PixelConstantBuffer[i] = std::make_unique<ConstantBuffer<PixelConstantBufferData>>();
    }
}

XMMATRIX Primitive::GetWorldMatrix() const
{
    XMMATRIX translation = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX scale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
    XMMATRIX world = scale * rotation * translation;
    return world;
}

void Primitive::UpdateVertexConstantBuffer()
{
	// Update vertex constant buffer with world, view, and projection matrices
	FVertexConstantBufferData vertexData;
	XMMATRIX world = GetWorldMatrix();
	XMStoreFloat4x4(&vertexData.WorldMTX, world);

	XMMATRIX view = GCamera.GetViewMatrix();
	XMStoreFloat4x4(&vertexData.ViewMTX, view);

	XMMATRIX projection = GCamera.GetProjectionMatrix();
	XMStoreFloat4x4(&vertexData.ProjectionMTX, projection);

	XMMATRIX worldViewProj = world * view * projection;
	XMStoreFloat4x4(&vertexData.WorldViewProjMTX, worldViewProj);
	
	VertexConstantBuffer[GSwapChain.GetFrameInFlightIndex()]->Update(vertexData);
}

void Primitive::UpdatePixelConstantBuffer()
{
    float speed = 0.5;
	PixelConstantBufferData pixelData;
	pixelData.Color.x = 0.5f + 0.5f * sinf(speed);
	pixelData.Color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	pixelData.Color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	pixelData.Color.w = 1.0f;
	PixelConstantBuffer[GSwapChain.GetFrameInFlightIndex()]->Update(pixelData);
}

void Primitive::UpdateConstantBuffers()
{
    UpdateVertexConstantBuffer();
    UpdatePixelConstantBuffer();
}

void Primitive::UploadVertexBuffer()
{
    std::vector<Vertex> vertexList;
    GenerateVertices(vertexList);
    const UINT vertsDataSize = static_cast<UINT>(sizeof(Vertex) * vertexList.size());
    VertexBuffer = UploadBuffer::Upload(vertexList.data(), vertsDataSize);
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
    IndexBuffer = UploadBuffer::Upload(indexList.data(), indexDataSize);
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
    GRHI.GetCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    GRHI.GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
    GRHI.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

std::vector<D3D12_INPUT_ELEMENT_DESC> Primitive::GetVertexLayout()
{
    return {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
}

D3D12_RESOURCE_DESC Primitive::CreateVertexBufferDesc(uint32_t VertexCount)
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
