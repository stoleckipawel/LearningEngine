#pragma once

#include "D3D12/Primitive.h"

/**
 * @brief PrimitiveBox - Represents a 3D cube/box primitive for rendering.
 *
 * Inherits from Primitive and provides mesh data for a unit cube centered at the origin.
 * Extend this class to create more complex box shapes or add custom attributes.
 */
class PrimitiveBox : public Primitive {
public:
    /**
     * @brief Construct a new PrimitiveBox object
     * @param translation Initial translation (position) of the box in world space
     * @param rotation Initial rotation (Euler angles, radians) of the box
     * @param scale Initial scale of the box
     *
     * All parameters default to identity (no translation, no rotation, unit scale).
     */
    PrimitiveBox(
        const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

protected:
    /**
     * @brief Generates the vertex data for the box.
     * @param outVertices Output vector to be filled with vertex data.
     *
     * Override this method to define custom vertex layouts for derived primitives.
     */
    void GenerateVertices(std::vector<Vertex>& outVertices) const override;

    /**
     * @brief Generates the index data for the box.
     * @param outIndices Output vector to be filled with index data.
     *
     * Override this method to define custom index layouts for derived primitives.
     */
    void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
