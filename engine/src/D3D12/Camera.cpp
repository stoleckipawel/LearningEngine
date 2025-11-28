#include "PCH.h"
#include "DX12/Camera.h"
#include "DX12/SwapChain.h"

using namespace DirectX;

// Global camera instance
Camera GCamera;

Camera::Camera()
    : m_position{ 0.0f, 0.0f, -4.0f }, m_rotationDegrees{ 0.0f, 0.0f, 0.0f }
{
}

// Sets the camera position in world space
void Camera::SetPosition(const XMFLOAT3& position)
{
    m_position = position;
}

// Sets the camera rotation (Euler angles in degrees)
void Camera::SetRotationDegrees(const XMFLOAT3& eulerDegrees)
{
    // Clamp or wrap angles to [0, 360)
    m_rotationDegrees.x = fmodf(eulerDegrees.x, 360.0f);
    if (m_rotationDegrees.x < 0) m_rotationDegrees.x += 360.0f;
    m_rotationDegrees.y = fmodf(eulerDegrees.y, 360.0f);
    if (m_rotationDegrees.y < 0) m_rotationDegrees.y += 360.0f;
    m_rotationDegrees.z = fmodf(eulerDegrees.z, 360.0f);
    if (m_rotationDegrees.z < 0) m_rotationDegrees.z += 360.0f;
}

// Moves the camera forward in its local space
void Camera::MoveForward(float distance)
{
    // Calculate forward vector from rotation
    XMMATRIX rotation = GetRotationMatrix();
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotation);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos = XMVectorAdd(pos, XMVectorScale(forward, distance));
    XMStoreFloat3(&m_position, pos);
}

// Moves the camera right in its local space
void Camera::MoveRight(float distance)
{
    XMMATRIX rotation = GetRotationMatrix();
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotation);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos = XMVectorAdd(pos, XMVectorScale(right, distance));
    XMStoreFloat3(&m_position, pos);
}

// Moves the camera up in its local space
void Camera::MoveUp(float distance)
{
    XMMATRIX rotation = GetRotationMatrix();
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotation);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos = XMVectorAdd(pos, XMVectorScale(up, distance));
    XMStoreFloat3(&m_position, pos);
}

XMFLOAT3 Camera::GetPosition() const
{
    return m_position;
}

// Returns the camera rotation (Euler angles in degrees)
XMFLOAT3 Camera::GetRotationDegrees() const
{
    return m_rotationDegrees;
}

// Returns the view matrix for the camera
XMMATRIX Camera::GetViewMatrix() const
{
    XMMATRIX rotation = GetRotationMatrix();
    XMVECTOR eyePosition = XMLoadFloat3(&m_position);
    XMVECTOR focusPoint = XMVectorAdd(eyePosition, XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotation));
    XMVECTOR upDirection = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotation);
    return XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
}

// Helper: Builds a rotation matrix from Euler angles in degrees
XMMATRIX Camera::GetRotationMatrix() const
{
    float pitch = XMConvertToRadians(m_rotationDegrees.x);
    float yaw   = XMConvertToRadians(m_rotationDegrees.y);
    float roll  = XMConvertToRadians(m_rotationDegrees.z);
    return XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    float NearZ = 0.01f;
    float FarZ = 10.0f;
    float fovY = XMConvertToRadians(60.0f);

    D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
    float aspectRatio = viewport.Width / viewport.Height;

    return XMMatrixPerspectiveFovLH(fovY, aspectRatio, NearZ, FarZ);
}