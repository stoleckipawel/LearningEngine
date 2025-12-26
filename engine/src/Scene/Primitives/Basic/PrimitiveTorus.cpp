#include "PCH.h"
#include "Primitives/Basic/PrimitiveTorus.h"

PrimitiveTorus::PrimitiveTorus(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Primitive(translation, rotation, scale)
{
}

void PrimitiveTorus::GenerateVertices(std::vector<Vertex>& outVertices) const
{
    const int major = 32;
    const int minor = 16;
    const float R = 1.0f; // major radius
    const float r = 0.3f; // minor radius

    outVertices.clear();
    outVertices.reserve(major * minor);

    for (int i = 0; i < major; ++i)
    {
        float u = (float)i / (float)major * DirectX::XM_2PI;
        float cu = std::cosf(u);
        float su = std::sinf(u);

        for (int j = 0; j < minor; ++j)
        {
            float v = (float)j / (float)minor * DirectX::XM_2PI;
            float cv = std::cosf(v);
            float sv = std::sinf(v);

            float x = (R + r * cv) * cu;
            float y = r * sv;
            float z = (R + r * cv) * su;

            DirectX::XMFLOAT3 pos{x, y, z};
            DirectX::XMFLOAT2 uv{(float)i / major, (float)j / minor};
            DirectX::XMFLOAT4 color{std::fabs(cu), std::fabs(sv), std::fabs(cv), 1.0f};

            outVertices.push_back({pos, uv, color});
        }
    }
}

void PrimitiveTorus::GenerateIndices(std::vector<DWORD>& outIndices) const
{
    const int major = 32;
    const int minor = 16;
    outIndices.clear();
    outIndices.reserve(major * minor * 6);

    for (int i = 0; i < major; ++i)
    {
        for (int j = 0; j < minor; ++j)
        {
            int nextI = (i + 1) % major;
            int nextJ = (j + 1) % minor;

            DWORD a = (DWORD)(i * minor + j);
            DWORD b = (DWORD)(nextI * minor + j);
            DWORD c = (DWORD)(i * minor + nextJ);
            DWORD d = (DWORD)(nextI * minor + nextJ);

            outIndices.push_back(a);
            outIndices.push_back(b);
            outIndices.push_back(c);

            outIndices.push_back(b);
            outIndices.push_back(d);
            outIndices.push_back(c);
        }
    }
}
