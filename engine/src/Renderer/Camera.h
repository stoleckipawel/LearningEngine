
#pragma once

#include "Event.h"
#include "DepthConvention.h"
#include <DirectXMath.h>
#include <cstdint>
#include "D3D12ConstantBufferData.h"

class Camera final
{
  public:
	[[nodiscard]] static Camera& Get() noexcept;

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(Camera&&) = delete;

	// Configuration
	void SetFovYDegrees(float fovDegrees) noexcept;
	void SetAspectRatio(float aspect) noexcept;
	void SetNearFar(float nearZ, float farZ) noexcept;

	// Transform setters
	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
	void SetRotationDegrees(const DirectX::XMFLOAT3& eulerDegrees) noexcept;  // pitch, yaw, roll
	void SetRotationQuaternion(const DirectX::XMFLOAT4& quat) noexcept;
	void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = {0.0f, 1.0f, 0.0f}) noexcept;

	// Transform modifiers
	void TranslateLocal(const DirectX::XMFLOAT3& delta) noexcept;  // in local space
	void MoveForward(float distance) noexcept;
	void MoveRight(float distance) noexcept;
	void MoveUp(float distance) noexcept;

	// Queries
	[[nodiscard]] DirectX::XMFLOAT3 GetPosition() const noexcept;
	[[nodiscard]] DirectX::XMFLOAT4 GetRotationQuaternion() const noexcept;
	[[nodiscard]] DirectX::XMFLOAT3 GetDirection() const noexcept;  // normalized forward (local +Z)
	[[nodiscard]] DirectX::XMFLOAT3 GetRight() const noexcept;
	[[nodiscard]] DirectX::XMFLOAT3 GetUp() const noexcept;
	[[nodiscard]] float GetFovYDegrees() const noexcept { return m_fovYDegrees; }
	[[nodiscard]] float GetNearZ() const noexcept { return m_nearZ; }
	[[nodiscard]] float GetFarZ() const noexcept { return m_farZ; }

	// Matrix accessors (cached)
	[[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const noexcept;
	[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix() const noexcept;

	// Get View Constant Buffer Data
	[[nodiscard]] PerViewConstantBufferData GetViewConstantBufferData() const noexcept;

	// Zero-copy helper: fill caller-provided POD with view constants.
	// Useful for avoiding an extra copy when uploading to a GPU upload buffer.
	void FillViewCB(PerViewConstantBufferData& out) const noexcept;

  private:
	Camera() noexcept;
	~Camera() noexcept;

	void InvalidateMatrices() noexcept;
	void InvalidateProjection() noexcept;
	void RebuildViewIfNeeded() const noexcept;
	void RebuildProjectionIfNeeded() const noexcept;
	void OnDepthModeChanged(DepthMode mode) noexcept;

	// mutable cache for lazy updates
	mutable DirectX::XMFLOAT4X4 m_viewMat;  // cached view
	mutable DirectX::XMFLOAT4X4 m_projMat;  // cached proj
	mutable bool m_bViewDirty = true;
	mutable bool m_bProjDirty = true;

	// transform state
	DirectX::XMFLOAT3 m_position{0.0f, 0.0f, -4.0f};
	DirectX::XMFLOAT4 m_orientation{0.0f, 0.0f, 0.0f, 1.0f};  // quaternion (x,y,z,w)

	// projection parameters
	float m_fovYDegrees = 60.0f;
	float m_aspect = 16.0f / 9.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 10000.0f;

	// Event subscription
	EventHandle m_depthModeChangedHandle;
};

inline Camera& GCamera = Camera::Get();