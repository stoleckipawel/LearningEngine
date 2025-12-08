#include "PCH.h"
#include "D3D12/PrimitivePlane.h"

PrimitivePlane::PrimitivePlane()
{
    
}

void PrimitivePlane::GenerateVertices(std::vector<Vertex>& outVertices) const {
    // Simple 2x2 plane in XZ
    outVertices = {
        { { -1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f }, { 1.0, 1.0, 1.0, 1.0 } }, // 0
        { { -1.0f, 0.0f,  1.0f }, { 0.0f, 0.0f }, { 1.0, 0.0, 0.0, 1.0 } }, // 1
        { {  1.0f, 0.0f,  1.0f }, { 1.0f, 0.0f }, { 0.0, 1.0, 0.0, 1.0 } }, // 2
        { {  1.0f, 0.0f, -1.0f }, { 1.0f, 1.0f }, { 0.0, 0.0, 1.0, 1.0 } }, // 3
    };
}

void PrimitivePlane::GenerateIndices(std::vector<DWORD>& outIndices) const {
    outIndices = 
    {
        0, 1, 2, 0, 2, 3
    };
}
