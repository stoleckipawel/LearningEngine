
#pragma once

#include <memory>
#include <vector>
#include "D3D12/Primitive.h"
#include "D3D12/PrimitiveBox.h"
#include "D3D12/PrimitivePlane.h"

/**
 * @brief Factory for creating and managing renderable primitives.
 *
 * Provides methods to append new primitives (box, plane, etc.), upload their resources,
 * and update their constant buffers each frame. Extend this class to add new primitive types
 * or custom management logic.
 */
class PrimitiveFactory {
public:
    /**
     * @brief Construct a new PrimitiveFactory object.
     */
    PrimitiveFactory() = default;
    ~PrimitiveFactory() = default;


    /**
     * @brief Append a new box primitive to the factory.
     * @param translation Initial translation (position) of the box in world space
     * @param rotation Initial rotation (Euler angles, radians) of the box
     * @param scale Initial scale of the box
     */
    void AppendBox(const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
                  const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
                  const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

    /**
     * @brief Append a new plane primitive to the factory.
     * @param translation Initial translation (position) of the plane in world space
     * @param rotation Initial rotation (Euler angles, radians) of the plane
     * @param scale Initial scale of the plane
     */
    void AppendPlane(const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
                    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
                    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

    /**
     * @brief Upload all primitives' geometry to the GPU.
     */
    void Upload();

    /**
     * @brief Get a const reference to the vector of all managed primitives.
     */
    const std::vector<std::unique_ptr<Primitive>>& GetPrimitives() const;

    /**
     * @brief Get a reference to the first primitive (for PSO setup, etc.).
     */
    Primitive& GetFirstPrimitive() const;

    /**
     * @brief Updates all primitives' constant buffers for the current frame.
     */
    void UpdateConstantBuffers();

private:
    /**
     * @brief Storage for all managed primitives.
     */
    std::vector<std::unique_ptr<Primitive>> m_primitives;
};

