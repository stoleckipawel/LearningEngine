// ============================================================================
// PrimitiveCapsule.h
// ----------------------------------------------------------------------------
// Capsule primitive mesh (cylinder with hemispherical caps).
// ============================================================================

#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Scene/Mesh.h"

class SPARKLE_ENGINE_API PrimitiveCapsule : public Mesh
{
  public:
	PrimitiveCapsule(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	void GenerateGeometry(MeshData& outMeshData) const override;
};
