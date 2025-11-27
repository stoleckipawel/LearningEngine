#pragma once

// Camera class manages position, rotation, and view/projection matrices for a 3D camera.
class Camera
{
public:
    Camera();

    // Sets camera position in world space
    void SetPosition(const DirectX::XMFLOAT3& position);
    // Sets camera rotation (Euler angles in degrees)
    void SetRotationDegrees(const DirectX::XMFLOAT3& eulerDegrees);

    // Moves camera forward in its local space
    void MoveForward(float distance);
    // Moves camera right in its local space
    void MoveRight(float distance);
    // Moves camera up in its local space
    void MoveUp(float distance);

    // Returns camera position
    DirectX::XMFLOAT3 GetPosition() const;
    // Returns camera rotation (Euler angles in degrees)
    DirectX::XMFLOAT3 GetRotationDegrees() const;
    // Returns view matrix for the camera
    DirectX::XMMATRIX GetViewMatrix() const;
    // Returns projection matrix for the camera
    DirectX::XMMATRIX GetProjectionMatrix() const;

private:
    // Camera position in world space
    DirectX::XMFLOAT3 m_position;
    // Camera rotation (Euler angles: x=pitch, y=yaw, z=roll)
    DirectX::XMFLOAT3 m_rotationDegrees;

    // Helper: Builds a rotation matrix from Euler angles
    DirectX::XMMATRIX GetRotationMatrix() const;
};


// Global camera instance
extern Camera GCamera;