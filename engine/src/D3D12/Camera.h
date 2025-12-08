
#pragma once
#include <DirectXMath.h>

// Camera: Manages position, rotation, and view/projection matrices for a 3D camera.
class Camera {
public:
    Camera() noexcept = default;

    // Sets the camera position in world space.
    void SetPosition(const XMFLOAT3& position) noexcept { m_position = position; }
    // Sets camera rotation (Euler angles in degrees).
    void SetRotationDegrees(const XMFLOAT3& eulerDegrees) noexcept;

    // Moves camera forward/right/up in its local space.
    void MoveForward(float distance) noexcept;
    void MoveRight(float distance) noexcept;
    void MoveUp(float distance) noexcept;

    // Returns camera position.
    XMFLOAT3 GetPosition() const noexcept { return m_position; }
    // Returns camera rotation (Euler angles in degrees).
    XMFLOAT3 GetRotationDegrees() const noexcept { return m_rotationDegrees; }
    // Returns view matrix for the camera.
    XMMATRIX GetViewMatrix() const noexcept;
    // Returns projection matrix for the camera.
    XMMATRIX GetProjectionMatrix() const noexcept;
    // Helper: Builds a rotation matrix from Euler angles in degrees.
    XMMATRIX GetRotationMatrix() const noexcept;
private:
    XMFLOAT3 m_position = {0.0f, 0.0f, -4.0f};        // Camera position in world space
    XMFLOAT3 m_rotationDegrees = {0.0f, 0.0f, 0.0f}; // Camera rotation (Euler angles: x=pitch, y=yaw, z=roll)
};

// Global camera instance
extern Camera GCamera;