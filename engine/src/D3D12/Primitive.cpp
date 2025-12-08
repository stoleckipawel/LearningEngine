
#include "PCH.h"
#include "D3D12/Primitive.h"
#include "D3D12/UploadBuffer.h"

Primitive::Primitive()
{

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
