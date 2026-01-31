// ============================================================================
// Mesh.h
// ----------------------------------------------------------------------------
// Base class for renderable meshes with transform, vertex/index buffer
// management, and GPU upload/binding.
//
// USAGE:
//   class MyMesh : public Mesh {
//       void GenerateVertices(std::vector<Vertex>& out) const override;
//       void GenerateIndices(std::vector<DWORD>& out) const override;
//   };
//   MyMesh mesh({0,0,0}, {0,0,0}, {1,1,1});
//   mesh.Upload(rhi);
//   mesh.Bind(commandList);
//
// DESIGN:
//   - NVI pattern: derived classes implement GenerateVertices/GenerateIndices
//   - Lazy world matrix caching via mutable members and dirty flag
//   - Transform API: TRS (Translation, Rotation-Euler, Scale)
//
// VERTEX FORMAT:
//   - Position (XMFLOAT3), UV (XMFLOAT2), Color (XMFLOAT4)
//   - Normal (XMFLOAT3), Tangent (XMFLOAT4) with handedness in W
//
// NOTES:
//   - Static GetStaticVertexLayout() provides allocation-free input layout
//   - Upload() must be called before Bind() for GPU resources
// ============================================================================

#pragma once

#include "D3D12Rhi.h"
#include "D3D12ConstantBufferData.h"
#include <DirectXMath.h>

#include <span>
#include <vector>

using Microsoft::WRL::ComPtr;

// ============================================================================
// Vertex Structure
// ============================================================================

/// Vertex structure for geometry with position, UV, color, normal, and tangent.
struct Vertex
{
	DirectX::XMFLOAT3 position;  ///< Vertex position (x, y, z)
	DirectX::XMFLOAT2 uv;        ///< Texture coordinates (u, v)
	DirectX::XMFLOAT4 color;     ///< Vertex color (r, g, b, a)
	DirectX::XMFLOAT3 normal;    ///< Vertex normal (x, y, z)
	DirectX::XMFLOAT4 tangent;   ///< Tangent (xyz=direction, w=handedness ±1)
};

// ============================================================================
// Mesh Base Class
// ============================================================================

/// Base class for renderable meshes. Handles upload/binding and per-frame resources.
class Mesh
{
  public:
	// ========================================================================
	// Static Utilities
	// ========================================================================

	/// Returns the default input layout for the engine's standard Vertex format.
	/// Allocation-free and suitable for PSO creation.
	static std::span<const D3D12_INPUT_ELEMENT_DESC> GetStaticVertexLayout() noexcept;

	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Constructs a mesh with optional TRS transform (defaults to identity).
	Mesh(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f}) noexcept;

	/// Virtual destructor for polymorphic base.
	virtual ~Mesh() = default;

	// ========================================================================
	// Transform API
	// ========================================================================

	/// Sets the world-space translation.
	void SetTranslation(const DirectX::XMFLOAT3& t) noexcept;

	/// Returns the current translation.
	DirectX::XMFLOAT3 GetTranslation() const noexcept;

	/// Sets rotation from Euler angles (radians).
	void SetRotationEuler(const DirectX::XMFLOAT3& r) noexcept;

	/// Returns the current Euler angles (radians).
	DirectX::XMFLOAT3 GetRotationEuler() const noexcept;

	/// Sets the scale factors.
	void SetScale(const DirectX::XMFLOAT3& s) noexcept;

	/// Returns the current scale.
	DirectX::XMFLOAT3 GetScale() const noexcept;

	// ========================================================================
	// Matrix Accessors
	// ========================================================================

	/// Computes world transformation matrix from TRS. Cached; rebuilds when dirty.
	DirectX::XMMATRIX GetWorldMatrix() const noexcept;

	/// Returns the 3x3 rotation-only matrix.
	DirectX::XMFLOAT3X3 GetWorldRotationMatrix3x3() const noexcept;

	/// Returns inverse-transpose of world for correct normal transformation in shaders.
	DirectX::XMMATRIX GetWorldInverseTransposeMatrix() const noexcept;

	// ========================================================================
	// GPU Operations
	// ========================================================================

	/// Fills per-object VS constant buffer data for shader upload.
	[[nodiscard]] PerObjectVSConstantBufferData GetPerObjectVSConstants() const noexcept;

	/// Returns the number of indices in the index buffer.
	UINT GetIndexCount() const noexcept { return m_indexCount; }

	/// Binds vertex/index buffers and primitive topology to command list.
	void Bind(ID3D12GraphicsCommandList* commandList) const noexcept;

	/// Uploads both vertex and index buffers to the GPU.
	void Upload(D3D12Rhi& rhi);

  protected:
	// ========================================================================
	// NVI: Geometry Generation (Override in Derived Classes)
	// ========================================================================

	/// Populates mesh vertex data. Called by Upload().
	virtual void GenerateVertices(std::vector<Vertex>& outVertices) const = 0;

	/// Populates mesh index data. Called by Upload().
	virtual void GenerateIndices(std::vector<DWORD>& outIndices) const = 0;

	// ------------------------------------------------------------------------
	// Upload Helpers
	// ------------------------------------------------------------------------

	/// Creates and uploads the vertex buffer resource.
	void UploadVertexBuffer(D3D12Rhi& rhi);

	/// Creates and uploads the index buffer resource.
	void UploadIndexBuffer(D3D12Rhi& rhi);

	/// Marks cached world matrix as invalid (called from setters).
	void InvalidateWorldCache() noexcept { m_bWorldDirty = true; }

  private:
	// ------------------------------------------------------------------------
	// Transform State
	// ------------------------------------------------------------------------

	DirectX::XMFLOAT3 m_translation{0.0f, 0.0f, 0.0f};    ///< World-space position
	DirectX::XMFLOAT3 m_rotationEuler{0.0f, 0.0f, 0.0f};  ///< Euler angles (radians)
	DirectX::XMFLOAT3 m_scale{1.0f, 1.0f, 1.0f};          ///< Scale factors

	// ------------------------------------------------------------------------
	// Cached World Matrix (mutable for lazy evaluation)
	// ------------------------------------------------------------------------

	mutable DirectX::XMFLOAT4X4 m_worldMatrixCache{};  ///< Cached world matrix
	mutable bool m_bWorldDirty = true;                 ///< True when matrix needs rebuild

	/// Rebuilds the cached world matrix when dirty.
	void RebuildWorldIfNeeded() const noexcept;

  protected:
	// ------------------------------------------------------------------------
	// GPU Resources
	// ------------------------------------------------------------------------

	ComPtr<ID3D12Resource2> VertexBuffer = nullptr;    ///< Vertex buffer resource
	ComPtr<ID3D12Resource2> IndexBuffer = nullptr;     ///< Index buffer resource
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};  ///< Vertex buffer view
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};    ///< Index buffer view
	UINT m_indexCount = 0;                             ///< Number of indices
};
