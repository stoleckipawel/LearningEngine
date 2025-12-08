#pragma once
#include <memory>
#include <vector>
#include "D3D12/Primitive.h"
#include "D3D12/PrimitiveBox.h"
#include "D3D12/PrimitivePlane.h"

class PrimitiveFactory {
public:
    PrimitiveFactory() = default;
    ~PrimitiveFactory() = default;

    void AppendBox();
    void AppendPlane();
    void Upload();
    const std::vector<std::unique_ptr<Primitive>>& GetPrimitives() const;
    Primitive& GetFirstPrimitive() const;
private:
    std::vector<std::unique_ptr<Primitive>> m_primitives;
};

