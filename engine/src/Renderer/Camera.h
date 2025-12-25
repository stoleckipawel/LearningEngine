
#pragma once

#include <DirectXMath.h>
#include <cstdint>
#include "D3D12ConstantBufferData.h"

class Camera
{
  public:
	Camera() noexcept;
	~Camera() = default;

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

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
	DirectX::XMFLOAT3 GetPosition() const noexcept;
	DirectX::XMFLOAT4 GetRotationQuaternion() const noexcept;
	DirectX::XMFLOAT3 GetDirection() const noexcept;  // normalized forward (local +Z)
	DirectX::XMFLOAT3 GetRight() const noexcept;
	DirectX::XMFLOAT3 GetUp() const noexcept;
	float GetNearZ() const noexcept { return m_nearZ; }
	float GetFarZ() const noexcept { return m_farZ; }

	// Matrix accessors (cached)
	DirectX::XMMATRIX GetViewMatrix() const noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() const noexcept;

	// Get View Constant Buffer Data
	PerViewConstantBufferData GetViewConstantBufferData() const noexcept;

	// Zero-copy helper: fill caller-provided POD with view constants.
	// Useful for avoiding an extra copy when uploading to a GPU upload buffer.
	void FillViewCB(PerViewConstantBufferData& out) const noexcept;

  private:
	void InvalidateMatrices() noexcept;
	void RebuildViewIfNeeded() const noexcept;
	void RebuildProjectionIfNeeded() const noexcept;

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
};

// Global camera instance
extern Camera GCamera;