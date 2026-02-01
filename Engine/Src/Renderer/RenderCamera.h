// ============================================================================
// RenderCamera.h
// ----------------------------------------------------------------------------
// Rendering-side camera representation. Reads from GameCamera and builds
// view/projection matrices for GPU submission.
//
// USAGE:
//   GameCamera gameCamera;
//   RenderCamera renderCamera(gameCamera);
//
//   // Each frame before rendering:
//   renderCamera.Update();  // Rebuilds matrices from GameCamera (if dirty)
//
//   // Use matrices for rendering:
//   XMMATRIX view = renderCamera.GetViewMatrix();
//   XMMATRIX proj = renderCamera.GetProjectionMatrix();
//
//   // Use frustum for culling:
//   const Frustum& frustum = renderCamera.GetFrustum();
//
// DESIGN:
//   - Pure rendering construct (no game logic, no setters)
//   - Constructed with a GameCamera reference (always valid)
//   - Reads ALL state from GameCamera (single source of truth)
//   - Only rebuilds matrices when GameCamera is dirty (optimization)
//   - Extracts frustum planes for culling
//
// ============================================================================

#pragma once

#include "D3D12ConstantBufferData.h"
#include "Core/Math/Frustum.h"
#include <DirectXMath.h>

class GameCamera;

// ============================================================================
// RenderCamera
// ============================================================================

class RenderCamera final
{
  public:
	explicit RenderCamera(GameCamera& gameCamera) noexcept;
	~RenderCamera() noexcept = default;

	RenderCamera(const RenderCamera&) = delete;
	RenderCamera& operator=(const RenderCamera&) = delete;
	RenderCamera(RenderCamera&&) = delete;
	RenderCamera& operator=(RenderCamera&&) = delete;

	/// Syncs state from game camera and rebuilds matrices if dirty.
	/// Call each frame before rendering.
	void Update() noexcept;

	/// Forces a full matrix rebuild regardless of dirty state.
	void ForceUpdate() noexcept;

	[[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const noexcept;
	[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix() const noexcept;
	[[nodiscard]] DirectX::XMMATRIX GetViewProjectionMatrix() const noexcept;

	/// Returns the view frustum for culling operations.
	[[nodiscard]] const Frustum& GetFrustum() const noexcept { return m_frustum; }

	/// Returns view constant buffer data for GPU upload.
	[[nodiscard]] PerViewConstantBufferData GetViewConstantBufferData() const noexcept;

  private:
	/// Rebuilds all matrices and frustum from GameCamera state.
	void RebuildMatrices() noexcept;

	GameCamera& m_gameCamera;

	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewProjMatrix;
	Frustum m_frustum;
};
