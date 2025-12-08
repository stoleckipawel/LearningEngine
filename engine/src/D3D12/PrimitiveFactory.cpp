

#include "PCH.h"
#include "D3D12/PrimitiveFactory.h"

void PrimitiveFactory::AppendBox()
{
    auto box = std::make_unique<PrimitiveBox>();
    m_primitives.push_back(std::move(box));
}

void PrimitiveFactory::AppendPlane()
{
    auto plane = std::make_unique<PrimitivePlane>();
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
