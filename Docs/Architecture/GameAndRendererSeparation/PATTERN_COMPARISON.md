# Scene/Renderer Separation Patterns: Comparison

## Overview

This document compares three industry-proven patterns for separating game logic (Scene/GameFramework) from rendering (Renderer/RHI).

| Pattern | Reference Engine | Core Idea |
|---------|-----------------|-----------|
| [Render Proxy](PATTERN_RENDER_PROXY.md) | Unreal Engine | Mirror each game component with a render proxy |
| [Data Extraction](PATTERN_DATA_EXTRACTION.md) | Unity DOTS | Extract flat arrays of data each frame |
| [RID Server](PATTERN_RID_SERVER.md) | Godot | Centralized server with opaque handles |

---

## Quick Comparison

| Criteria | Render Proxy | Data Extraction | RID Server |
|----------|--------------|-----------------|------------|
| **Decoupling** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Performance** | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Complexity** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Memory** | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Threading** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Debugging** | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐ |

---

## Detailed Comparison

### Decoupling Quality

| Pattern | Scene Module Knows | Renderer Module Knows |
|---------|-------------------|----------------------|
| **Render Proxy** | Nothing about GPU | Nothing about game logic |
| **Data Extraction** | Shared buffer types | Shared buffer types |
| **RID Server** | Handle types only | Everything about GPU |

**Winner**: Tie between Render Proxy and RID Server

### Performance Characteristics

| Pattern | Per-Frame Cost | Memory Access | Scalability |
|---------|---------------|---------------|-------------|
| **Render Proxy** | Sync dirty objects | Random (object-based) | O(dirty objects) |
| **Data Extraction** | Copy all visible | Sequential (cache-friendly) | O(visible objects) |
| **RID Server** | Command dispatch | Random (hash lookup) | O(commands) |

**Winner**: Data Extraction for cache-friendly iteration

### Implementation Complexity

| Pattern | New Component Steps | Boilerplate | Learning Curve |
|---------|---------------------|-------------|----------------|
| **Render Proxy** | Create component + proxy + sync | High | Steep |
| **Data Extraction** | Add fields to arrays | Low | Gentle |
| **RID Server** | Add server methods + handle type | Medium | Medium |

**Winner**: Data Extraction for simplicity

### Threading Model

| Pattern | Game Thread | Render Thread | Sync Point |
|---------|-------------|---------------|------------|
| **Render Proxy** | Owns components | Owns proxies | Once per frame |
| **Data Extraction** | Writes buffers | Reads buffers | Buffer swap |
| **RID Server** | Calls server | Server internal | Per-call or batched |

**Winner**: Render Proxy for explicit thread ownership

---

## Decision Matrix

### Choose Render Proxy When:

- [x] Building AAA-scale engine
- [x] Multi-threaded rendering is required
- [x] Large team with specialized roles
- [x] Complex per-object render state
- [x] Need fine-grained dirty tracking

### Choose Data Extraction When:

- [x] Performance is critical (mobile, VR)
- [x] Using ECS/Data-Oriented Design
- [x] Many similar objects in scene
- [x] Cache optimization matters
- [x] Simple, uniform object types

### Choose RID Server When:

- [x] Heavy scripting integration needed
- [x] Building editor/tools
- [x] Hot-reload support required
- [x] Complete GPU abstraction required
- [x] Need serialization-friendly handles

---

## Hybrid Approaches

Most production engines combine patterns:

### Unreal's Hybrid (Proxy + Batching)
```
Game Components → Render Proxies → Mesh Batches → Draw Calls
                    (Pattern 1)       (Pattern 2)
```

### Unity's Hybrid (Extraction + Handles)
```
ECS Components → FrameData Extraction → Batch Buffers
                    (Pattern 2)
RenderTexture → Handle System → GPU Resources
                    (Pattern 3)
```

---

## Recommendation for Sparkle Engine

Based on:
- Learning/educational focus
- Mid-size project scope
- Single developer
- Need for clear, debuggable code

### Recommended: Modified Data Extraction + Asset/View

```
┌─────────────────────────────────────────────────────────────────┐
│                        Asset Layer                              │
│   MeshAsset, TextureAsset, MaterialAsset (CPU data)            │
├─────────────────────────────────────────────────────────────────┤
│                        Scene Layer                              │
│   Entity (transform + asset IDs, NO GPU types)                 │
├─────────────────────────────────────────────────────────────────┤
│                        Render Layer                             │
│   GpuResourceCache (maps asset IDs → GPU resources)            │
│   RenderView (extracts visible entities + resolves GPU refs)   │
└─────────────────────────────────────────────────────────────────┘
```

**Why?**
1. Clear three-tier separation
2. Easy to understand and debug
3. Good performance with lazy caching
4. Extensible for future features
5. Matches existing module structure

---

## Migration Path

### Current State
```cpp
// Scene owns GPU addresses (wrong!)
m_scene->GetMeshFactory().Upload(*m_rhi);
```

### Target State
```cpp
// Scene has NO GPU knowledge
auto& entity = m_scene->CreateEntity();
entity.SetMeshAssetId(AssetId::Cube);
entity.SetTransform(transform);

// Renderer resolves assets to GPU resources
RenderView view;
view.Build(*m_scene, m_gpuResourceCache);
m_renderer->Render(view);
```

### Steps
1. Create `MeshAsset` (CPU data) in Assets module
2. Create `Entity` (logic only) in Scene module
3. Create `GpuMesh` + `GpuResourceCache` in Renderer
4. Create `RenderView` to bridge Scene → Renderer
5. Remove all GPU code from Scene module

---

## File References

- [Pattern 1: Render Proxy](PATTERN_RENDER_PROXY.md)
- [Pattern 2: Data Extraction](PATTERN_DATA_EXTRACTION.md)
- [Pattern 3: RID Server](PATTERN_RID_SERVER.md)
