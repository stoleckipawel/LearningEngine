// ============================================================================
// PrimitiveBox.h
// ----------------------------------------------------------------------------
// Unit cube primitive mesh centered at origin.
// ============================================================================

#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Scene/Mesh.h"

class SPARKLE_ENGINE_API PrimitiveBox : public Mesh
{
  public:
	// Construct a new PrimitiveBox object. Parameters default to identity (no transform).
	PrimitiveBox(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	// Generate the geometry data for the box.
	void GenerateGeometry(MeshData& outMeshData) const override;
};
