#pragma once

#include "D3D12Rhi.h"
#include <DirectXMath.h>

#include <span>
#include <vector>


using Microsoft::WRL::ComPtr;

// Vertex structure for geometry: position, UV, and color.

struct Vertex
{
	DirectX::XMFLOAT3 position;  // Vertex position (x, y, z)
	DirectX::XMFLOAT2 uv;        // Texture coordinates (u, v)
	DirectX::XMFLOAT4 color;     // Vertex color (r, g, b, a)
	DirectX::XMFLOAT3 normal;   // Vertex normal (x, y, z)
	DirectX::XMFLOAT4 tangent;// Vertex tangent (x, y, z, w) — xyz is the tangent direction, w is the handedness sign (+1 or -1).
};

// Base class for renderable primitives. Handles upload/binding and per-frame resources.
class Primitive
{
  public:
	// Default input layout for the engine's standard Vertex format.
	// This is allocation-free and suitable for PSO creation.
	static std::span<const D3D12_INPUT_ELEMENT_DESC> GetStaticVertexLayout() noexcept;

	// Construct a new Primitive. translation/rotation/scale default to identity (no transform).
	Primitive(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Virtual destructor for polymorphic base
	virtual ~Primitive() = default;

	// Transform API:  explicit setters/getters
	void SetTranslation(const DirectX::XMFLOAT3& t) noexcept;
	DirectX::XMFLOAT3 GetTranslation() const noexcept;

	void SetRotationEuler(const DirectX::XMFLOAT3& r) noexcept;  // Euler angles (radians)
	DirectX::XMFLOAT3 GetRotationEuler() const noexcept;

	void SetScale(const DirectX::XMFLOAT3& s) noexcept;
	DirectX::XMFLOAT3 GetScale() const noexcept;

	// Compute the world transformation matrix from TRS and return it. This is cached
	// internally and lazily rebuilt on transform changes to avoid repeated work.
	DirectX::XMMATRIX GetWorldMatrix() const noexcept;

	// Return the 3x3 rotation-only matrix.
	DirectX::XMFLOAT3X3 GetWorldRotationMatrix3x3() const noexcept;

	// Return inverse-transpose of world for correct normal transformation in shaders.
	DirectX::XMMATRIX GetWorldInverseTransposeMatrix() const noexcept;

	// Return the number of indices in the index buffer.
	UINT GetIndexCount() const noexcept { return m_indexCount; }

	// Set geometry buffers and topology for rendering. Override to bind more resources.
	virtual void Set();

	// Return the input layout for the vertex structure. Override to customize.
	virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout() const;

	// Create a D3D12 resource description for a vertex buffer with VertexCount vertices.
	virtual D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount) const;

	// Upload both vertex and index buffers to the GPU.
	void Upload();

  protected:
	// Must be implemented by derived classes to populate mesh vertex data into outVertices.
	virtual void GenerateVertices(std::vector<Vertex>& outVertices) const = 0;

	// Must be implemented by derived classes to populate mesh index data into outIndices.
	virtual void GenerateIndices(std::vector<DWORD>& outIndices) const = 0;

	// Upload the vertex buffer for the geometry.
	void UploadVertexBuffer();

	// Upload the index buffer for the geometry.
	void UploadIndexBuffer();

	// Internal: mark cached world invalid (called from setters)
	void InvalidateWorldCache() noexcept { m_bWorldDirty = true; }

  private:
	// Transform state (encapsulated)
	XMFLOAT3 m_translation{0.0f, 0.0f, 0.0f};
	XMFLOAT3 m_rotationEuler{0.0f, 0.0f, 0.0f};
	XMFLOAT3 m_scale{1.0f, 1.0f, 1.0f};

	// Cached world matrix to avoid recomputing each time. Mutable so const accessors
	// can rebuild lazily without exposing mutability to callers.
	mutable XMFLOAT4X4 m_worldMatrixCache{};
	mutable bool m_bWorldDirty = true;

	// Lazy rebuild helper: rebuild the cached world matrix when dirty.
	void RebuildWorldIfNeeded() const noexcept;

  protected:
	// GPU resources and views
	ComPtr<ID3D12Resource2> VertexBuffer = nullptr;    //< Vertex buffer resource
	ComPtr<ID3D12Resource2> IndexBuffer = nullptr;     //< Index buffer resource
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};  //< Vertex buffer view
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};    //< Index buffer view
	UINT m_indexCount = 0;                             //< Number of indices in the index buffer
};
