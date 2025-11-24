#include "Camera.h"
#include <DirectXMath.h>
#include <cmath>
#include "SwapChain.h"

using namespace DirectX;

Camera GCamera;

Camera::Camera()
    : m_position{ 0.0f, 0.0f, -2.0f }, m_rotationDegrees{ 0.0f, 0.0f, 0.0f }
{
}

void Camera::SetPosition(const XMFLOAT3& position)
{
    m_position = position;
}

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

void Camera::MoveForward(float distance)
{
    // Calculate forward vector from rotation
    XMMATRIX rot = GetRotationMatrix();
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rot);
    XMVECTOR pos = XMLoadFloat3(&m_position) + XMVector3Normalize(forward) * distance;
    XMStoreFloat3(&m_position, pos);
}

void Camera::MoveRight(float distance)
{
    XMMATRIX rot = GetRotationMatrix();
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rot);
    XMVECTOR pos = XMLoadFloat3(&m_position) + XMVector3Normalize(right) * distance;
    XMStoreFloat3(&m_position, pos);
}

void Camera::MoveUp(float distance)
{
    XMMATRIX rot = GetRotationMatrix();
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rot);
    XMVECTOR pos = XMLoadFloat3(&m_position) + XMVector3Normalize(up) * distance;
    XMStoreFloat3(&m_position, pos);
}

XMFLOAT3 Camera::GetPosition() const
{
    return m_position;
}

XMFLOAT3 Camera::GetRotationDegrees() const
{
    return m_rotationDegrees;
}

XMMATRIX Camera::GetViewMatrix() const
{
    XMMATRIX rot = GetRotationMatrix();
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rot);
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rot);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    XMVECTOR target = pos + forward;
    return XMMatrixLookAtLH(pos, target, up);
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

// Helper: Build rotation matrix from Euler angles in degrees
XMMATRIX Camera::GetRotationMatrix() const
{
    float pitch = XMConvertToRadians(m_rotationDegrees.x);
    float yaw = XMConvertToRadians(m_rotationDegrees.y);
    float roll = XMConvertToRadians(m_rotationDegrees.z);
    return XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}
