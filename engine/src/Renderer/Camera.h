// ============================================================================
// Camera.h
// ----------------------------------------------------------------------------
// Perspective camera for 3D rendering with lazy matrix caching.
//
// USAGE:
//   GCamera.SetPosition({0.0f, 2.0f, -5.0f});
//   GCamera.LookAt({0.0f, 0.0f, 0.0f});
//   auto viewMat = GCamera.GetViewMatrix();
//   auto projMat = GCamera.GetProjectionMatrix();
//
// DESIGN:
//   - Lazy evaluation: matrices recomputed only when dirty flags are set
//   - Quaternion-based orientation avoids gimbal lock
//   - Subscribes to depth convention changes for projection matrix updates
//
// COORDINATE SYSTEM:
//   - Right-handed: +X right, +Y up, +Z out of screen (camera looks -Z)
//   - Quaternion orientation stored as (x, y, z, w)
//
// NOTES:
//   - Singleton accessed via GCamera global reference
//   - Thread-unsafe: matrices cached with mutable members
// ============================================================================

#pragma once

#include "Event.h"
#include "DepthConvention.h"
#include <DirectXMath.h>
#include <cstdint>
#include "D3D12ConstantBufferData.h"

class Camera final
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Returns the singleton Camera instance.
	[[nodiscard]] static Camera& Get() noexcept;

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(Camera&&) = delete;

	// ========================================================================
	// Configuration
	// ========================================================================

	/// Sets the vertical field of view in degrees.
	void SetFovYDegrees(float fovDegrees) noexcept;

	/// Sets the aspect ratio (width / height).
	void SetAspectRatio(float aspect) noexcept;

	/// Sets near and far clipping planes.
	void SetNearFar(float nearZ, float farZ) noexcept;

	// ========================================================================
	// Transform Setters
	// ========================================================================

	/// Sets the world-space position of the camera.
	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;

	/// Sets rotation from Euler angles (pitch, yaw, roll) in degrees.
	void SetRotationDegrees(const DirectX::XMFLOAT3& eulerDegrees) noexcept;

	/// Sets rotation from a quaternion (x, y, z, w).
	void SetRotationQuaternion(const DirectX::XMFLOAT4& quat) noexcept;

	/// Orients the camera to look at a target point.
	void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = {0.0f, 1.0f, 0.0f}) noexcept;

	// ========================================================================
	// Transform Modifiers
	// ========================================================================

	/// Translates the camera in its local coordinate space.
	void TranslateLocal(const DirectX::XMFLOAT3& delta) noexcept;

	/// Moves the camera along its forward axis.
	void MoveForward(float distance) noexcept;

	/// Moves the camera along its right axis.
	void MoveRight(float distance) noexcept;

	/// Moves the camera along its up axis.
	void MoveUp(float distance) noexcept;

	// ========================================================================
	// Queries
	// ========================================================================

	/// Returns the world-space position.
	[[nodiscard]] DirectX::XMFLOAT3 GetPosition() const noexcept;

	/// Returns the orientation as a quaternion.
	[[nodiscard]] DirectX::XMFLOAT4 GetRotationQuaternion() const noexcept;

	/// Returns the normalized forward direction (local -Z in world space).
	[[nodiscard]] DirectX::XMFLOAT3 GetDirection() const noexcept;

	/// Returns the normalized right direction (local +X in world space).
	[[nodiscard]] DirectX::XMFLOAT3 GetRight() const noexcept;

	/// Returns the normalized up direction (local +Y in world space).
	[[nodiscard]] DirectX::XMFLOAT3 GetUp() const noexcept;

	[[nodiscard]] float GetFovYDegrees() const noexcept { return m_fovYDegrees; }
	[[nodiscard]] float GetNearZ() const noexcept { return m_nearZ; }
	[[nodiscard]] float GetFarZ() const noexcept { return m_farZ; }

	// ========================================================================
	// Matrix Accessors
	// ========================================================================

	/// Returns the view matrix (world-to-camera transform). Cached; rebuilds if dirty.
	[[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const noexcept;

	/// Returns the projection matrix. Cached; rebuilds if dirty.
	[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix() const noexcept;

	// ========================================================================
	// Constant Buffer Helpers
	// ========================================================================

	/// Returns view constant buffer data for GPU upload.
	[[nodiscard]] PerViewConstantBufferData GetViewConstantBufferData() const noexcept;

	/// Fills caller-provided struct with view constants (zero-copy pattern).
	/// Useful for avoiding an extra copy when uploading to a GPU upload buffer.
	void FillViewCB(PerViewConstantBufferData& out) const noexcept;

  private:
	Camera() noexcept;
	~Camera() noexcept;

	// ------------------------------------------------------------------------
	// Matrix Cache Invalidation
	// ------------------------------------------------------------------------

	void InvalidateMatrices() noexcept;
	void InvalidateProjection() noexcept;
	void RebuildViewIfNeeded() const noexcept;
	void RebuildProjectionIfNeeded() const noexcept;
	void OnDepthModeChanged(DepthMode mode) noexcept;

	// ------------------------------------------------------------------------
	// Cached Matrices (mutable for lazy evaluation)
	// ------------------------------------------------------------------------

	mutable DirectX::XMFLOAT4X4 m_viewMat;   ///< Cached view matrix
	mutable DirectX::XMFLOAT4X4 m_projMat;   ///< Cached projection matrix
	mutable bool m_bViewDirty = true;        ///< True when view matrix needs rebuild
	mutable bool m_bProjDirty = true;        ///< True when projection matrix needs rebuild

	// ------------------------------------------------------------------------
	// Transform State
	// ------------------------------------------------------------------------

	DirectX::XMFLOAT3 m_position{0.0f, 0.0f, -4.0f};           ///< World-space position
	DirectX::XMFLOAT4 m_orientation{0.0f, 0.0f, 0.0f, 1.0f};   ///< Quaternion (x, y, z, w)

	// ------------------------------------------------------------------------
	// Projection Parameters
	// ------------------------------------------------------------------------

	float m_fovYDegrees = 60.0f;       ///< Vertical field of view in degrees
	float m_aspect = 16.0f / 9.0f;     ///< Aspect ratio (width / height)
	float m_nearZ = 0.01f;             ///< Near clipping plane
	float m_farZ = 10000.0f;           ///< Far clipping plane

	// ------------------------------------------------------------------------
	// Event Subscription
	// ------------------------------------------------------------------------

	EventHandle m_depthModeChangedHandle;  ///< Handle for depth convention change events
};

/// Global singleton reference for convenient access.
inline Camera& GCamera = Camera::Get();