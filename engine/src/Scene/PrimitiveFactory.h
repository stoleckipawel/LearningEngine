
#pragma once

#include <memory>
#include <vector>
#include "Primitive.h"
#include "PrimitiveBox.h"
#include "PrimitivePlane.h"

// Factory for creating and managing renderable primitives; append, upload, and update them.
class PrimitiveFactory
{
  public:
	// Construct a new PrimitiveFactory object.
	PrimitiveFactory() = default;
	~PrimitiveFactory() = default;

	// Append a new box primitive to the factory.
	void AppendBox(const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	               const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	               const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Append a new plane primitive to the factory.
	void AppendPlane(const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	                 const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	                 const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Upload all primitives geometry to the GPU.
	void Upload();

	// Get const reference to the vector of all managed primitives.
	const std::vector<std::unique_ptr<Primitive>>& GetPrimitives() const;

	// Get reference to the first primitive (useful for PSO setup).
	Primitive& GetFirstPrimitive() const;

  private:
	// Storage for all managed primitives.
	std::vector<std::unique_ptr<Primitive>> m_primitives;
};
