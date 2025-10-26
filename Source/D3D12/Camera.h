#pragma once
#include "../Vendor/Windows/WinInclude.h"

class Camera
{
public:
    Camera();

    // Set camera position and rotation (Euler angles in degrees)
    void SetPosition(const DirectX::XMFLOAT3& position);
    void SetRotationDegrees(const DirectX::XMFLOAT3& eulerDegrees);

    // Move camera in local space
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);

    // Getters
    DirectX::XMFLOAT3 GetPosition() const;
    DirectX::XMFLOAT3 GetRotationDegrees() const;
    DirectX::XMMATRIX GetViewMatrix() const;

private:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotationDegrees; // Euler angles: x=pitch, y=yaw, z=roll

    DirectX::XMMATRIX GetRotationMatrix() const; // Helper for internal use
};
