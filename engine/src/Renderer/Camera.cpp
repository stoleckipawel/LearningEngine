#include "PCH.h"
#include "Camera.h"
#include "D3D12SwapChain.h"
#include "DepthConvention.h"

using namespace DirectX;

Camera& Camera::Get() noexcept
{
	static Camera instance;
	return instance;
}

Camera::Camera() noexcept
{
	XMStoreFloat4x4(&m_viewMat, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projMat, XMMatrixIdentity());

	// Add listener for depth mode changes
	m_depthModeChangedHandle = DepthConvention::OnModeChanged.Add(
	    [this](DepthMode)
	    {
		    OnDepthModeChanged(DepthMode{});
	    });
}

Camera::~Camera() noexcept
{
	DepthConvention::OnModeChanged.Remove(m_depthModeChangedHandle);
}

void Camera::SetFovYDegrees(float fovDegrees) noexcept
{
	m_fovYDegrees = fovDegrees;
	m_bProjDirty = true;
}

void Camera::SetAspectRatio(float aspect) noexcept
{
	m_aspect = aspect;
	m_bProjDirty = true;
}

void Camera::SetNearFar(float nearZ, float farZ) noexcept
{
	m_nearZ = nearZ;
	m_farZ = farZ;
	m_bProjDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& position) noexcept
{
	m_position = position;
	InvalidateMatrices();
}

void Camera::SetRotationDegrees(const XMFLOAT3& eulerDegrees) noexcept
{
	// Convert degrees to radians and create quaternion
	const float pitch = XMConvertToRadians(eulerDegrees.x);
	const float yaw = XMConvertToRadians(eulerDegrees.y);
	const float roll = XMConvertToRadians(eulerDegrees.z);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMStoreFloat4(&m_orientation, quat);
	InvalidateMatrices();
}

void Camera::SetRotationQuaternion(const XMFLOAT4& quat) noexcept
{
	m_orientation = quat;
	InvalidateMatrices();
}

void Camera::LookAt(const XMFLOAT3& target, const XMFLOAT3& up) noexcept
{
	// Build a look-at view matrix from current eye position -> target.
	// We then invert the view (to obtain world) and extract a quaternion
	// representing camera orientation. This keeps orientation and position
	// consistent for subsequent local-space motion (TranslateLocal/Move*).
	const XMVECTOR eyePositionVec = XMLoadFloat3(&m_position);
	const XMVECTOR targetVec = XMLoadFloat3(&target);
	const XMVECTOR upDirectionVec = XMLoadFloat3(&up);

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePositionVec, targetVec, upDirectionVec);

	// Invert view -> world and extract rotation. Using matrix->quat is
	// numerically stable for rigid transforms and is efficient with
	// DirectXMath intrinsics.
	XMMATRIX worldMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMVECTOR rotationQuat = XMQuaternionRotationMatrix(worldMatrix);
	XMStoreFloat4(&m_orientation, rotationQuat);

	// Mark dependent cached data dirty so callers get fresh matrices/CB.
	InvalidateMatrices();
}

void Camera::TranslateLocal(const XMFLOAT3& delta) noexcept
{
	// Translate in the camera's local space. We rotate the provided delta by
	// the camera quaternion and add to position. This keeps the API simple
	// and avoids constructing intermediate matrices.
	const XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	const XMVECTOR deltaVec = XMLoadFloat3(&delta);
	const XMVECTOR movedVec = XMVector3Rotate(deltaVec, rotationQuat);
	XMVECTOR positionVec = XMLoadFloat3(&m_position);
	positionVec = XMVectorAdd(positionVec, movedVec);
	XMStoreFloat3(&m_position, positionVec);
	InvalidateMatrices();
}

void Camera::MoveForward(float distance) noexcept
{
	XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	XMVECTOR forwardVec = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat);
	XMVECTOR positionVec = XMLoadFloat3(&m_position);
	positionVec = XMVectorAdd(positionVec, XMVectorScale(forwardVec, distance));
	XMStoreFloat3(&m_position, positionVec);
	InvalidateMatrices();
}

void Camera::MoveRight(float distance) noexcept
{
	XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	XMVECTOR rightVec = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat);
	XMVECTOR positionVec = XMLoadFloat3(&m_position);
	positionVec = XMVectorAdd(positionVec, XMVectorScale(rightVec, distance));
	XMStoreFloat3(&m_position, positionVec);
	InvalidateMatrices();
}

void Camera::MoveUp(float distance) noexcept
{
	XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	XMVECTOR upVec = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat);
	XMVECTOR positionVec = XMLoadFloat3(&m_position);
	positionVec = XMVectorAdd(positionVec, XMVectorScale(upVec, distance));
	XMStoreFloat3(&m_position, positionVec);
	InvalidateMatrices();
}

XMFLOAT3 Camera::GetDirection() const noexcept
{
	// Returns the camera's forward vector in world space (normalized).
	// Constructed from the orientation quaternion and normalized for safety.
	const XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	XMVECTOR forwardVec = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationQuat);
	forwardVec = XMVector3Normalize(forwardVec);
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, forwardVec);
	return dir;
}

XMFLOAT3 Camera::GetRight() const noexcept
{
	XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	XMVECTOR rightVec = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat);
	XMFLOAT3 r;
	XMStoreFloat3(&r, rightVec);
	return r;
}

XMFLOAT3 Camera::GetUp() const noexcept
{
	XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	XMVECTOR upVec = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat);
	XMFLOAT3 u;
	XMStoreFloat3(&u, upVec);
	return u;
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return m_position;
}

DirectX::XMFLOAT4 Camera::GetRotationQuaternion() const noexcept
{
	return m_orientation;
}

XMMATRIX Camera::GetViewMatrix() const noexcept
{
	RebuildViewIfNeeded();
	return XMLoadFloat4x4(&m_viewMat);
}

XMMATRIX Camera::GetProjectionMatrix() const noexcept
{
	RebuildProjectionIfNeeded();
	return XMLoadFloat4x4(&m_projMat);
}

void Camera::InvalidateMatrices() noexcept
{
	m_bViewDirty = true;
}

void Camera::InvalidateProjection() noexcept
{
	m_bProjDirty = true;
}

void Camera::OnDepthModeChanged([[maybe_unused]] DepthMode mode) noexcept
{
	InvalidateProjection();
}

void Camera::RebuildViewIfNeeded() const noexcept
{
	if (!m_bViewDirty)
		return;
	const XMVECTOR positionVec = XMLoadFloat3(&m_position);
	const XMVECTOR rotationQuat = XMLoadFloat4(&m_orientation);
	const XMVECTOR forwardVec = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat);
	const XMVECTOR upVec = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat);
	const XMVECTOR targetVec = XMVectorAdd(positionVec, forwardVec);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(positionVec, targetVec, upVec);
	XMStoreFloat4x4(&m_viewMat, viewMatrix);
	m_bViewDirty = false;
}

void Camera::RebuildProjectionIfNeeded() const noexcept
{
	if (!m_bProjDirty)
		return;

	const float fovRadians = XMConvertToRadians(m_fovYDegrees);
	XMMATRIX projMatrix = DepthConvention::CreatePerspectiveFovLH(fovRadians, m_aspect, m_nearZ, m_farZ);
	XMStoreFloat4x4(&m_projMat, projMatrix);
	m_bProjDirty = false;
}
