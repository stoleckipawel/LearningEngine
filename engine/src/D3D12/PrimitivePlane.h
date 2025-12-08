#pragma once

#include "D3D12/Primitive.h"

/**
 * @brief PrimitivePlane - Represents a simple 2D plane primitive for rendering.
 *
 * Inherits from Primitive and provides mesh data for a flat plane in the XZ plane.
 */
class PrimitivePlane : public Primitive 
{
public:
    /**
     * @brief Construct a new PrimitivePlane object
     * @param translation Initial translation (position) of the plane in world space
     * @param rotation Initial rotation (Euler angles, radians) of the plane
     * @param scale Initial scale of the plane
     *
     * All parameters default to identity (no translation, no rotation, unit scale).
     */
    PrimitivePlane(
        const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

protected:
    /**
     * @brief Generates the vertex data for the plane.
     * @param outVertices Output vector to be filled with vertex data.
     *
     * Override this method to define custom vertex layouts for derived primitives.
     */
    void GenerateVertices(std::vector<Vertex>& outVertices) const override;

    /**
     * @brief Generates the index data for the plane.
     * @param outIndices Output vector to be filled with index data.
     *
     * Override this method to define custom index layouts for derived primitives.
     */
    void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
