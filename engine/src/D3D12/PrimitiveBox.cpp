#include "PCH.h"
#include "D3D12/PrimitiveBox.h"

PrimitiveBox::PrimitiveBox(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
    : Primitive(translation, rotation, scale)
{
}

void PrimitiveBox::GenerateVertices(std::vector<Vertex>& outVertices) const {
    outVertices = {
        // Front face
        { { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f }, { 1.0, 0.0, 0.0, 1.0 } }, // 0
        { { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f }, { 0.0, 1.0, 0.0, 1.0 } }, // 1
        { {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f }, { 0.0, 0.0, 1.0, 1.0 } }, // 2
        { {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f }, { 1.0, 1.0, 0.0, 1.0 } }, // 3
        // Back face
        { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0, 0.0, 1.0, 1.0 } }, // 4
        { { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f }, { 0.0, 1.0, 1.0, 1.0 } }, // 5
        { {  1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f }, { 1.0, 1.0, 1.0, 1.0 } }, // 6
        { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f }, { 0.5, 0.5, 0.5, 1.0 } }, // 7
    };
}

void PrimitiveBox::GenerateIndices(std::vector<DWORD>& outIndices) const {
    outIndices = {
        // Front face (reversed winding)
        0, 2, 1, 0, 3, 2,
        // Back face (reversed winding)
        4, 5, 6, 4, 6, 7,
        // Left face (reversed winding)
        4, 1, 5, 4, 0, 1,
        // Right face (reversed winding)
        3, 6, 2, 3, 7, 6,
        // Top face (reversed winding)
        1, 6, 5, 1, 2, 6,
        // Bottom face (reversed winding)
        4, 3, 0, 4, 7, 3
    };
}
