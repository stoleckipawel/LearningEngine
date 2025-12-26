#include "PCH.h"
#include "Primitives/Basic/PrimitiveCone.h"

PrimitiveCone::PrimitiveCone(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Primitive(translation, rotation, scale)
{
}

void PrimitiveCone::GenerateVertices(std::vector<Vertex>& outVertices) const
{
    const int slices = 32;
    outVertices.clear();
    outVertices.reserve(slices + 2);

    // Apex at y=1
    DirectX::XMFLOAT3 apex{0.0f, 1.0f, 0.0f};
    outVertices.push_back({apex, {0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});

    // Base ring at y=-1
    for (int i = 0; i < slices; ++i)
    {
        float phi = (float)i / (float)slices * DirectX::XM_2PI;
        float x = std::cosf(phi);
        float z = std::sinf(phi);
        DirectX::XMFLOAT3 pos{x, -1.0f, z};
        DirectX::XMFLOAT2 uv{(float)i / (float)slices, 1.0f};
        DirectX::XMFLOAT4 color{std::fabs(x), 0.5f, std::fabs(z), 1.0f};
        outVertices.push_back({pos, uv, color});
    }

    // Base center
    DirectX::XMFLOAT3 baseCenter{0.0f, -1.0f, 0.0f};
    outVertices.push_back({baseCenter, {0.5f, 0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}});
}

void PrimitiveCone::GenerateIndices(std::vector<DWORD>& outIndices) const
{
    const int slices = 32;
    outIndices.clear();
    outIndices.reserve(slices * 6);

    // Apex is vertex 0, ring starts at 1, base center is last
    DWORD apex = 0;
    DWORD baseStart = 1;
    DWORD baseCenter = (DWORD)(1 + slices);

    for (int i = 0; i < slices; ++i)
    {
        DWORD next = (DWORD)(baseStart + ((i + 1) % slices));
        DWORD cur = (DWORD)(baseStart + i);

        // side triangle (cur, next, apex) - use winding consistent with others
        outIndices.push_back(cur);
        outIndices.push_back(next);
        outIndices.push_back(apex);

        // base triangle (baseCenter, next, cur)
        outIndices.push_back(baseCenter);
        outIndices.push_back(next);
        outIndices.push_back(cur);
    }
}
