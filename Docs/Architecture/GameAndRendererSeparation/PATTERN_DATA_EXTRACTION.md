# Pattern 2: Data Extraction (Unity DOTS Style)

## Reference
Unity's DOTS/ECS with `RenderMesh` and `LocalToWorld` components.

## Overview

Game systems write to **shared data buffers**. The renderer reads from these buffers. No object-oriented proxies - just flat arrays of data optimized for cache-friendly iteration.

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Game Systems   │────▶│  Shared Buffers │◀────│    Renderer     │
│  (write only)   │     │  - Transforms   │     │  (read only)    │
│                 │     │  - Visibility   │     │                 │
└─────────────────┘     └─────────────────┘     └─────────────────┘
```

## Key Concepts

1. **Shared Buffers**: Flat arrays in Core module, visible to both sides
2. **Data Extraction**: Scene extracts relevant data into buffers each frame
3. **Read-Only Rendering**: Renderer only reads, never modifies game data
4. **Cache-Friendly**: Parallel arrays optimize CPU cache usage

## Implementation

### Shared Data Buffers (Core Module)

```cpp
// Core/Public/Rendering/RenderData.h
namespace Core
{

/// Shared render data buffer - written by game, read by renderer
/// Designed for cache-friendly iteration during rendering
struct CORE_API RenderBatch
{
    // Parallel arrays - same index = same object
    std::vector<Matrix4x4> WorldMatrices;
    std::vector<uint32_t> MeshIds;
    std::vector<uint32_t> MaterialIds;
    std::vector<uint8_t> VisibilityFlags;

    void Clear();
    void Reserve(size_t count);
    [[nodiscard]] size_t Count() const { return WorldMatrices.size(); }
};

/// Frame render data - extracted from scene each frame
struct CORE_API FrameRenderData
{
    RenderBatch OpaqueBatch;
    RenderBatch TransparentBatch;
    
    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
    Vector3 CameraPosition;

    void Clear();
};

} // namespace Core
```

### Data Extractor (Scene Module)

```cpp
// Scene/Public/Systems/RenderDataExtractor.h
namespace Scene
{

/// Extracts render-relevant data from Scene into shared buffer
/// Scene module owns this - Renderer just reads the result
class SCENE_API RenderDataExtractor
{
public:
    /// Extract all render data from scene into output buffer
    static void Extract(const Scene& scene, Core::FrameRenderData& outData);
};

} // namespace Scene
```

```cpp
// Scene/Private/Systems/RenderDataExtractor.cpp
void RenderDataExtractor::Extract(const Scene& scene, Core::FrameRenderData& outData)
{
    outData.Clear();

    // Extract camera data
    const auto& camera = scene.GetCamera();
    outData.ViewMatrix = camera.GetViewMatrix();
    outData.ProjectionMatrix = camera.GetProjectionMatrix();
    outData.CameraPosition = camera.GetPosition();

    // Extract mesh instances into flat arrays
    for (const auto& entity : scene.GetEntities())
    {
        if (!entity.IsVisible())
            continue;

        auto& batch = entity.IsTransparent() 
            ? outData.TransparentBatch 
            : outData.OpaqueBatch;

        batch.WorldMatrices.push_back(entity.GetWorldMatrix());
        batch.MeshIds.push_back(entity.GetMeshId());
        batch.MaterialIds.push_back(entity.GetMaterialId());
        batch.VisibilityFlags.push_back(1);
    }
}
```

### Renderer Consumption

```cpp
// Renderer/Private/Renderer.cpp
void Renderer::Render(const Core::FrameRenderData& frameData)
{
    // Update per-view constants
    m_viewConstants.ViewMatrix = frameData.ViewMatrix;
    m_viewConstants.ProjectionMatrix = frameData.ProjectionMatrix;

    // Render opaque batch
    const auto& opaque = frameData.OpaqueBatch;
    for (size_t i = 0; i < opaque.Count(); ++i)
    {
        if (!opaque.VisibilityFlags[i])
            continue;

        const GpuMesh* mesh = m_resourceCache.GetMesh(opaque.MeshIds[i]);
        const GpuMaterial* mat = m_resourceCache.GetMaterial(opaque.MaterialIds[i]);

        m_objectConstants.WorldMatrix = opaque.WorldMatrices[i];
        
        DrawMesh(mesh, mat);
    }
}
```

## Data Flow

```
1. Game Logic Updates
   └── Entity transforms, visibility, etc.

2. Frame Start - Extraction
   └── RenderDataExtractor::Extract(scene, frameData)
       ├── Clear previous frame data
       ├── Copy camera matrices
       └── Iterate entities → populate parallel arrays

3. Rendering (read-only)
   └── Renderer::Render(frameData)
       └── Iterate arrays, draw each item
```

## Memory Layout

```
Traditional (AoS - Array of Structures):
┌────────────────────────────────────────────────────────┐
│ Entity0: [Matrix|MeshId|MatId|Vis] │ Entity1: [...] │ ...
└────────────────────────────────────────────────────────┘
Cache miss when iterating matrices!

Data Extraction (SoA - Structure of Arrays):
┌─────────────────────────────────────┐
│ Matrices:  [M0][M1][M2][M3][M4]...  │  ← Cache-friendly iteration
├─────────────────────────────────────┤
│ MeshIds:   [0] [1] [0] [2] [1]...   │
├─────────────────────────────────────┤
│ MaterialIds:[0] [0] [1] [1] [0]...  │
└─────────────────────────────────────┘
```

## Pros

| Advantage | Description |
|-----------|-------------|
| **Cache-Friendly** | Parallel arrays maximize CPU cache hits |
| **Simple Mental Model** | Write → Extract → Read |
| **Easy Parallelization** | Arrays easily split across threads |
| **Minimal Coupling** | Core module defines data, both sides use it |
| **No Proxy Boilerplate** | Just add fields to arrays |

## Cons

| Disadvantage | Description |
|--------------|-------------|
| **Data Copying** | Full extraction every frame |
| **Less Flexible** | Hard to query specific objects during render |
| **Memory Bandwidth** | Large copies for big scenes |
| **Fixed Schema** | Adding new data requires Core changes |

## When to Use

- Performance-critical engines (mobile, VR)
- ECS/Data-Oriented Design architectures
- Scenes with many similar objects
- When cache optimization matters more than flexibility

## Module Dependencies

```
         Core Module
         ├── RenderData.h (shared types)
         │
    ┌────┴────┐
    ▼         ▼
Scene       Renderer
(writes)    (reads)
```
