#include "PCH.h"
#include "RenderCamera.h"
#include "Scene/Camera/GameCamera.h"
#include "DepthConvention.h"

using namespace DirectX;

// ============================================================================
// RenderCamera Implementation
// ============================================================================

RenderCamera::RenderCamera(GameCamera& gameCamera) noexcept : m_gameCamera(gameCamera)
{
	RebuildMatrices();
}

void RenderCamera::Update() noexcept
{
	if (m_gameCamera.IsDirty())
	{
		RebuildMatrices();
		m_gameCamera.ClearDirty();
	}
}

void RenderCamera::ForceUpdate() noexcept
{
	RebuildMatrices();
	m_gameCamera.ClearDirty();
}

void RenderCamera::RebuildMatrices() noexcept
{
	// Build view matrix
	const XMFLOAT3 position = m_gameCamera.GetPosition();
	const XMFLOAT3& direction = m_gameCamera.GetDirection();

	const XMVECTOR positionVec = XMLoadFloat3(&position);
	const XMVECTOR directionVec = XMLoadFloat3(&direction);
	const XMVECTOR targetVec = XMVectorAdd(positionVec, directionVec);

	// Use world up as hint - XMMatrixLookAtLH internally orthonormalizes
	const XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	const XMMATRIX view = XMMatrixLookAtLH(positionVec, targetVec, worldUp);
	XMStoreFloat4x4(&m_viewMatrix, view);

	// Build projection matrix
	const float fovRadians = XMConvertToRadians(m_gameCamera.GetFovYDegrees());
	const float aspect = m_gameCamera.GetAspectRatio();
	const float nearZ = m_gameCamera.GetNearZ();
	const float farZ = m_gameCamera.GetFarZ();

	const XMMATRIX proj = DepthConvention::CreatePerspectiveFovLH(fovRadians, aspect, nearZ, farZ);
	XMStoreFloat4x4(&m_projectionMatrix, proj);

	// Cache viewProj
	const XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMStoreFloat4x4(&m_viewProjMatrix, viewProj);

	// Extract frustum planes from combined view-projection matrix
	m_frustum.ExtractFromViewProjection(m_viewProjMatrix);
}

XMMATRIX RenderCamera::GetViewMatrix() const noexcept
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

XMMATRIX RenderCamera::GetProjectionMatrix() const noexcept
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

XMMATRIX RenderCamera::GetViewProjectionMatrix() const noexcept
{
	return XMLoadFloat4x4(&m_viewProjMatrix);
}

PerViewConstantBufferData RenderCamera::GetViewConstantBufferData() const noexcept
{
	PerViewConstantBufferData data = {};

	// DirectXMath uses row-major storage, HLSL declares row_major - no transpose needed
	XMStoreFloat4x4(&data.ViewMTX, GetViewMatrix());
	XMStoreFloat4x4(&data.ProjectionMTX, GetProjectionMatrix());
	XMStoreFloat4x4(&data.ViewProjMTX, GetViewProjectionMatrix());

	// Camera transform data from game camera
	data.CameraPosition = m_gameCamera.GetPosition();
	data.CameraDirection = m_gameCamera.GetDirection();
	data.NearZ = m_gameCamera.GetNearZ();
	data.FarZ = m_gameCamera.GetFarZ();

	return data;
}
