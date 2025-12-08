
#include "PCH.h"
#include "D3D12/PrimitiveFactory.h"

void PrimitiveFactory::UpdateConstantBuffers()
{
    for (size_t i = 0; i < m_primitives.size(); ++i)
    {
        m_primitives[i]->UpdateConstantBuffers();
    }
}

void PrimitiveFactory::AppendBox(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
    auto box = std::make_unique<PrimitiveBox>(translation, rotation, scale);
    m_primitives.push_back(std::move(box));
}

void PrimitiveFactory::AppendPlane(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
    auto plane = std::make_unique<PrimitivePlane>(translation, rotation, scale);
    m_primitives.push_back(std::move(plane));
}

const std::vector<std::unique_ptr<Primitive>>& PrimitiveFactory::GetPrimitives() const
{
    return m_primitives;
}

Primitive& PrimitiveFactory::GetFirstPrimitive() const
{
    return *m_primitives.front();
}

void PrimitiveFactory::Upload()
{
    for (auto& primitive : m_primitives)
    {
        if (primitive)
        {
            primitive->Upload();
        }    
    }
}
