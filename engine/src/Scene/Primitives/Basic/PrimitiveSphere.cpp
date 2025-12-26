#include "PCH.h"
#include "Primitives/Basic/PrimitiveSphere.h"

PrimitiveSphere::PrimitiveSphere(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Primitive(translation, rotation, scale)
{
}

void PrimitiveSphere::GenerateVertices(std::vector<Vertex>& outVertices) const
{
    // UV sphere parameterization
    const int latSegments = 16;
    const int lonSegments = 16;
    outVertices.clear();
    outVertices.reserve((latSegments + 1) * (lonSegments + 1));

    for (int lat = 0; lat <= latSegments; ++lat)
    {
        float theta = (float)lat / (float)latSegments * DirectX::XM_PI; // 0..PI
        float sinTheta = std::sinf(theta);
        float cosTheta = std::cosf(theta);

        for (int lon = 0; lon <= lonSegments; ++lon)
        {
            float phi = (float)lon / (float)lonSegments * DirectX::XM_2PI; // 0..2PI
            float sinPhi = std::sinf(phi);
            float cosPhi = std::cosf(phi);

            float x = sinTheta * cosPhi;
            float y = cosTheta;
            float z = sinTheta * sinPhi;

            DirectX::XMFLOAT3 pos{x, y, z};
            DirectX::XMFLOAT2 uv{(float)lon / (float)lonSegments, (float)lat / (float)latSegments};
            DirectX::XMFLOAT4 color{std::fabs(x), std::fabs(y), std::fabs(z), 1.0f};

            outVertices.push_back({pos, uv, color});
        }
    }
}

void PrimitiveSphere::GenerateIndices(std::vector<DWORD>& outIndices) const
{
    const int latSegments = 16;
    const int lonSegments = 16;
    outIndices.clear();
    outIndices.reserve(latSegments * lonSegments * 6);

    for (int lat = 0; lat < latSegments; ++lat)
    {
        for (int lon = 0; lon < lonSegments; ++lon)
        {
            DWORD first = (DWORD)(lat * (lonSegments + 1) + lon);
            DWORD second = (DWORD)((lat + 1) * (lonSegments + 1) + lon);

            // Two triangles per quad on the sphere surface
            outIndices.push_back(first);
            outIndices.push_back(second);
            outIndices.push_back(first + 1);

            outIndices.push_back(second);
            outIndices.push_back(second + 1);
            outIndices.push_back(first + 1);
        }
    }
}
