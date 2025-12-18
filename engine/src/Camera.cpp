#include "PCH.h"
#include "Camera.h"
#include "SwapChain.h"

using namespace DirectX;

// Global camera instance
Camera GCamera;

// Sets the camera rotation (Euler angles in degrees)
void Camera::SetRotationDegrees(const XMFLOAT3& eulerDegrees) noexcept
{
    // Clamp or wrap angles to [0, 360)
    auto wrap = [](float angle) noexcept {
        float a = fmodf(angle, 360.0f);
        return a < 0.0f ? a + 360.0f : a;
    };
    m_rotationDegrees.x = wrap(eulerDegrees.x);
    m_rotationDegrees.y = wrap(eulerDegrees.y);
    m_rotationDegrees.z = wrap(eulerDegrees.z);
}

// Moves the camera forward in its local space
void Camera::MoveForward(float distance) noexcept
{
    const XMMATRIX rotation = GetRotationMatrix();
    const XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotation);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos = XMVectorAdd(pos, XMVectorScale(forward, distance));
    XMStoreFloat3(&m_position, pos);
}

// Moves the camera right in its local space
void Camera::MoveRight(float distance) noexcept
{
    const XMMATRIX rotation = GetRotationMatrix();
    const XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotation);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos = XMVectorAdd(pos, XMVectorScale(right, distance));
    XMStoreFloat3(&m_position, pos);
}

// Moves the camera up in its local space
void Camera::MoveUp(float distance) noexcept
{
    const XMMATRIX rotation = GetRotationMatrix();
    const XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotation);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos = XMVectorAdd(pos, XMVectorScale(up, distance));
    XMStoreFloat3(&m_position, pos);
}


// Returns the view matrix for the camera
XMMATRIX Camera::GetViewMatrix() const noexcept
{
    const XMMATRIX rotation = GetRotationMatrix();
    const XMVECTOR eyePosition = XMLoadFloat3(&m_position);
    const XMVECTOR focusPoint = XMVectorAdd(eyePosition, XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotation));
    const XMVECTOR upDirection = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotation);
    return XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
}

// Helper: Builds a rotation matrix from Euler angles in degrees
XMMATRIX Camera::GetRotationMatrix() const noexcept
{
    const float pitch = XMConvertToRadians(m_rotationDegrees.x);
    const float yaw   = XMConvertToRadians(m_rotationDegrees.y);
    const float roll  = XMConvertToRadians(m_rotationDegrees.z);
    return XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

XMMATRIX Camera::GetProjectionMatrix() const noexcept
{
    constexpr float NearZ = 0.01f;
    constexpr float FarZ = 10000.0f;
    constexpr float fovY = XMConvertToRadians(60.0f); // 60 degree vertical FOV

    const D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
    const float aspectRatio = viewport.Width / viewport.Height;

    // Use perspective projection (not orthographic)
    return XMMatrixPerspectiveFovLH(fovY, aspectRatio, NearZ, FarZ);
}