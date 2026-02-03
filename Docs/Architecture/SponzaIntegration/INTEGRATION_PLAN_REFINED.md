# Sponza Integration Plan — Frame Graph Architecture

> **Purpose:** Render the Sponza scene using an AMD GPUOpen-inspired Frame Graph with Frostbite-style RenderContext, establishing a scalable rendering architecture.

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current State Analysis](#current-state-analysis)
3. [Target Architecture](#target-architecture)
4. [Phase Overview](#phase-overview)
5. [Phase -1: GameFramework Decoupling](#phase--1-gameframework-decoupling)
6. [Phase 0: RenderContext API](#phase-0-rendercontext-api)
7. [Phase 1: Frame Graph Foundation](#phase-1-frame-graph-foundation)
8. [Phase 2: SceneView Extraction](#phase-2-sceneview-extraction)
9. [Phase 3: glTF Loading](#phase-3-gltf-loading)
10. [Phase 4: ForwardOpaquePass](#phase-4-forwardopaquepass)
11. [Phase 5: Renderer Integration](#phase-5-renderer-integration)
12. [Phase 6: Lighting & Polish](#phase-6-lighting--polish)
13. [Module Ownership Diagrams](#module-ownership-diagrams)
14. [Success Criteria](#success-criteria)
15. [Learning Resources](#learning-resources)

---

## Executive Summary

### Goals
| Goal | Description |
|------|-------------|
| **Primary** | Render Sponza scene with basic directional lighting |
| **Architecture** | Introduce Frame Graph pattern (AMD GPUOpen inspired) |
| **Decoupling** | Remove all D3D12 dependencies from GameFramework |
| **Scope** | Minimal viable pipeline — forward rendering only |

### Non-Goals (Future Work)
- Deferred rendering / G-Buffer
- Shadow maps
- Resource aliasing & automatic barriers
- Multiple render passes (transparency, post-processing)
- Async compute

### Timeline
| Phase | Days | Hours | Gate |
|-------|------|-------|------|
| Phase -1: Decoupling | 1-2 | 8-10h | Box scene renders unchanged |
| Phase 0: RenderContext | 0.5 | 3-4h | API compiles |
| Phase 1: Frame Graph | 0.5 | 3-4h | Empty graph runs |
| Phase 2: SceneView | 0.5 | 2-3h | Extraction works |
| Phase 3: glTF Loading | 1 | 4-5h | Sponza data loads |
| Phase 4: ForwardOpaquePass | 1 | 4-5h | Pass renders geometry |
| Phase 5: Integration | 0.5 | 2-3h | Frame Graph renders scene |
| Phase 6: Lighting | 0.5 | 2-3h | Sponza lit correctly |
| **Total** | **5-6** | **~30h** | **Sponza renders lit** |

---

## Current State Analysis

### The Problem: Coupling

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     CURRENT ARCHITECTURE (Problematic)                   │
│                                                                          │
│   GameFramework                           RHI                            │
│   ┌──────────────────────┐               ┌──────────────────────┐       │
│   │ Mesh.h               │               │ D3D12Rhi.h           │       │
│   │ ├─ #include D3D12Rhi ├───────────────┤ ├─ Device            │       │
│   │ ├─ ComPtr<IBuffer>   │               │ ├─ CommandList       │       │
│   │ ├─ Upload()          │               │ └─ CreateBuffer()    │       │
│   │ └─ Bind()            │               └──────────────────────┘       │
│   └──────────────────────┘                                               │
│              │                                                           │
│              │ ❌ GameFramework depends on RHI                          │
│              │ ❌ GPU code scattered across modules                     │
│              │ ❌ Hard to test without GPU                              │
│              │ ❌ Cannot swap rendering backend                         │
│              ▼                                                           │
│   ┌──────────────────────┐                                               │
│   │ MeshFactory.h        │                                               │
│   │ └─ Upload(D3D12Rhi&) │ ← Calls into RHI module                      │
│   └──────────────────────┘                                               │
└─────────────────────────────────────────────────────────────────────────┘
```

### Files Currently Violating Module Boundaries

| File | Violation | Impact |
|------|-----------|--------|
| `Mesh.h` | `#include "D3D12Rhi.h"`, ComPtr members | CPU mesh class owns GPU resources |
| `MeshFactory.h` | `Upload(D3D12Rhi&)` method | Factory knows about GPU |
| `App.h` | Creates `D3D12Rhi` | App owns GPU instead of Renderer |

---

## Target Architecture

### Clean Module Separation

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                          TARGET ARCHITECTURE                                         │
│                                                                                      │
│                              ┌─────────────┐                                        │
│                              │    Core     │                                        │
│                              │  (Logging,  │                                        │
│                              │   Math)     │                                        │
│                              └──────┬──────┘                                        │
│                                     │                                               │
│              ┌──────────────────────┼──────────────────────┐                        │
│              │                      │                      │                        │
│              ▼                      ▼                      ▼                        │
│     ┌────────────────┐     ┌────────────────┐     ┌────────────────┐               │
│     │ GameFramework  │     │      RHI       │     │   Platform     │               │
│     │                │     │    (D3D12)     │     │   (Window)     │               │
│     │ • Mesh (CPU)   │     │                │     │                │               │
│     │ • Scene        │     │ • Device       │     │ • Window       │               │
│     │ • Camera       │     │ • Buffers      │     │ • Input        │               │
│     │ • GltfLoader   │     │ • Swapchain    │     │                │               │
│     │                │     │                │     │                │               │
│     │ ❌ NO D3D12    │     │                │     │                │               │
│     └───────┬────────┘     └───────┬────────┘     └───────┬────────┘               │
│             │                      │                      │                        │
│             │            ╔═════════╧═════════╗            │                        │
│             │            ║                   ║            │                        │
│             └────────────╫───────────────────╫────────────┘                        │
│                          ║                   ║                                      │
│                          ▼                   ▼                                      │
│                    ┌─────────────────────────────────┐                              │
│                    │           Renderer              │                              │
│                    │                                 │                              │
│                    │  OWNS:                          │                              │
│                    │  ├── D3D12Rhi (creates it!)     │                              │
│                    │  ├── RenderContext              │ ← NEW                        │
│                    │  ├── GPUMeshCache               │                              │
│                    │  ├── FrameGraph                 │                              │
│                    │  └── All GPU resources          │                              │
│                    │                                 │                              │
│                    │  READS (via SceneView):         │                              │
│                    │  └── Scene data                 │                              │
│                    └─────────────────────────────────┘                              │
│                                                                                      │
│   RULES:                                                                            │
│   ✅ Renderer → RHI, GameFramework, Platform                                        │
│   ✅ GameFramework → Core only                                                      │
│   ❌ GameFramework → RHI  (FORBIDDEN)                                               │
│   ❌ RHI → GameFramework  (FORBIDDEN)                                               │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### Data Flow Per Frame

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              PER-FRAME DATA FLOW                                     │
│                                                                                      │
│   1. GAME UPDATE                                                                    │
│   ───────────────                                                                   │
│   GameFramework updates scene, camera, objects                                      │
│                                                                                      │
│                              │                                                       │
│                              ▼                                                       │
│                                                                                      │
│   2. SCENE EXTRACTION                                                               │
│   ───────────────────                                                               │
│   ┌─────────────────┐          ┌─────────────────┐                                  │
│   │     Scene       │          │   SceneView     │                                  │
│   │                 │   ────►  │   (POD struct)  │                                  │
│   │ Camera, Meshes  │   Copy   │                 │                                  │
│   │ Materials       │   Data   │ No D3D12 types  │                                  │
│   └─────────────────┘          └────────┬────────┘                                  │
│                                         │                                           │
│   ════════════════════════════ BOUNDARY │ ═══════════════════════════════════════  │
│   GameFramework                         │                            Renderer       │
│                                         ▼                                           │
│                                                                                      │
│   3. FRAME GRAPH                                                                    │
│   ──────────────                                                                    │
│   ┌─────────────────────────────────────────────────────────────────────────┐      │
│   │  FrameGraph                                                              │      │
│   │  │                                                                       │      │
│   │  ├── Setup(sceneView)    → Passes declare resource needs                │      │
│   │  ├── Compile()           → (MVP: no-op, future: barriers)               │      │
│   │  └── Execute(context)    → Passes record commands via RenderContext     │      │
│   │                                                                          │      │
│   │  Passes:                                                                 │      │
│   │  └── ForwardOpaquePass                                                   │      │
│   │      ├── Setup: Request backbuffer, depth                               │      │
│   │      └── Execute: Draw all meshDraws                                    │      │
│   └─────────────────────────────────────────────────────────────────────────┘      │
│                                                                                      │
│   4. GPU EXECUTION                                                                  │
│   ────────────────                                                                  │
│   RenderContext → CommandList → GPU                                                 │
│   Present swapchain                                                                 │
│                                                                                      │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Phase Overview

### Dependency Graph

```
┌───────────────────────────────────────────────────────────────────────────────────┐
│                          PHASE DEPENDENCIES                                        │
│                                                                                    │
│   ┌─────────────────┐                                                              │
│   │  Phase -1       │ ──────────────────────────────────────────────────────┐     │
│   │  Decoupling     │                                                       │     │
│   │                 │                                                       │     │
│   │  • MeshData.h   │                                                       │     │
│   │  • GPUMesh      │                                                       │     │
│   │  • GPUMeshCache │                                                       │     │
│   │  • RHI → Renderer                                                       │     │
│   └────────┬────────┘                                                       │     │
│            │                                                                │     │
│            ▼                                                                │     │
│   ┌─────────────────┐                                                       │     │
│   │  Phase 0        │                                                       │     │
│   │  RenderContext  │                                                       │     │
│   │                 │                                                       │     │
│   │  • High-level   │                                                       │     │
│   │    command API  │                                                       │     │
│   │  • ResourceState│                                                       │     │
│   └────────┬────────┘                                                       │     │
│            │                                                                │     │
│            ▼                                                                │     │
│   ┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐      │     │
│   │  Phase 1        │ ──► │  Phase 2        │ ──► │  Phase 3        │      │     │
│   │  Frame Graph    │     │  SceneView      │     │  glTF Loading   │      │     │
│   │                 │     │                 │     │                 │      │     │
│   │  • RenderPass   │     │  • SceneView    │     │  • GltfLoader   │      │     │
│   │  • PassBuilder  │     │  • MeshDraw     │     │  • cgltf        │      │     │
│   │  • FrameGraph   │     │  • Extract()    │     │  • Materials    │      │     │
│   └────────┬────────┘     └─────────────────┘     └────────┬────────┘      │     │
│            │                                               │               │     │
│            └───────────────────────┬───────────────────────┘               │     │
│                                    │                                       │     │
│                                    ▼                                       │     │
│                           ┌─────────────────┐                              │     │
│                           │  Phase 4        │◄─────────────────────────────┘     │
│                           │  ForwardOpaque  │                                     │
│                           │                 │                                     │
│                           │  • Concrete pass│                                     │
│                           │  • Uses context │                                     │
│                           │  • GPUMeshCache │                                     │
│                           └────────┬────────┘                                     │
│                                    │                                              │
│                                    ▼                                              │
│                           ┌─────────────────┐                                     │
│                           │  Phase 5        │                                     │
│                           │  Integration    │                                     │
│                           │                 │                                     │
│                           │  • Wire up all  │                                     │
│                           │  • Renderer loop│                                     │
│                           └────────┬────────┘                                     │
│                                    │                                              │
│                                    ▼                                              │
│                           ┌─────────────────┐                                     │
│                           │  Phase 6        │                                     │
│                           │  Lighting       │                                     │
│                           │                 │                                     │
│                           │  • Sun light CB │                                     │
│                           │  • Shader update│                                     │
│                           │  ★ SPONZA LIT!  │                                     │
│                           └─────────────────┘                                     │
└───────────────────────────────────────────────────────────────────────────────────┘
```

### What We Build Now vs Later

| Component | MVP (Now) | Future Enhancement |
|-----------|-----------|-------------------|
| **RenderContext** | Thin wrapper over D3D12 command list | Multi-backend (Vulkan, etc.) |
| **ResourceState** | API-agnostic state enum | Full barrier automation |
| **ResourceHandle** | Fixed indices (0=back, 1=depth) | Virtual handles with aliasing |
| **FrameGraph::Compile()** | No-op | Topological sort, barriers |
| **Passes** | ForwardOpaquePass only | Shadow, GBuffer, Post, UI |
| **SceneView** | Camera + meshes + sun | Point lights, sky, particles |
| **GPUMeshCache** | Pointer-keyed lazy upload | Mesh IDs, streaming |

---

## Phase -1: GameFramework Decoupling

> **Goal:** Remove all D3D12 dependencies from GameFramework. Renderer owns all GPU resources.

### Step -1.1: Create MeshData (Pure CPU Vertex Data)

Create a new file with pure CPU data structures. No D3D12 dependencies.

**File:** `Engine/GameFramework/Public/Scene/MeshData.h`

| Structure | Members | Notes |
|-----------|---------|-------|
| **VertexData** | position (float3), uv (float2), color (float4), normal (float3), tangent (float4) | w = handedness |
| **MeshData** | vertices (array), indices (array) | Pure CPU, no buffers |

**MeshData Methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `IsValid()` | bool | True if has vertices and indices |
| `GetVertexCount()` | uint32 | Number of vertices |
| `GetIndexCount()` | uint32 | Number of indices |
| `GetVertexBufferSize()` | size_t | vertices.count × sizeof(VertexData) |
| `GetIndexBufferSize()` | size_t | indices.count × sizeof(uint32) |

---

### Step -1.2: Refactor Mesh Class (Remove GPU Code)

**File:** `Engine/GameFramework/Public/Scene/Mesh.h`

**Remove:**
- `#include "D3D12Rhi.h"`
- `#include "D3D12ConstantBufferData.h"`
- `m_vertexBuffer : ComPtr<...>`
- `m_indexBuffer : ComPtr<...>`
- `m_vertexBufferView : D3D12_VERTEX_BUFFER_VIEW`
- `m_indexBufferView : D3D12_INDEX_BUFFER_VIEW`
- `Upload(D3D12Rhi&)` method
- `Bind(CommandList*)` method

**Keep:**
- `m_translation, m_rotation, m_scale`
- `m_materialId`
- `GetWorldMatrix()`
- `GenerateGeometry()` (NVI pattern)

**Add:**
- `#include "MeshData.h"`
- `m_meshData : MeshData`
- `GetMeshData() : const MeshData&`

---

### Step -1.3: Refactor MeshFactory (Remove Upload)

**File:** `Engine/GameFramework/Public/Scene/MeshFactory.h`

**Remove:**
- `Upload(D3D12Rhi&)` method

**Keep:**
- `Rebuild(shape, count, config)`
- `AppendShape(...)`
- `Clear()`
- `GetMeshes()`

---

### Step -1.4: Create GPUMesh (New File in Renderer)

**File:** `Engine/Renderer/Public/GPUMesh.h`

| Member | Type | Description |
|--------|------|-------------|
| m_vertexBuffer | ComPtr<ID3D12Resource2> | GPU vertex buffer |
| m_indexBuffer | ComPtr<ID3D12Resource2> | GPU index buffer |
| m_vertexBufferView | D3D12_VERTEX_BUFFER_VIEW | For IASetVertexBuffers |
| m_indexBufferView | D3D12_INDEX_BUFFER_VIEW | For IASetIndexBuffer |
| m_indexCount | uint32 | For DrawIndexedInstanced |

| Method | Description |
|--------|-------------|
| `Upload(rhi, meshData)` | Create GPU buffers from MeshData |
| `Bind(cmdList)` | Call IASetVertexBuffers, IASetIndexBuffer |
| `GetIndexCount()` | Return m_indexCount |

**Upload Algorithm:**
```
Upload(rhi, meshData):
    1. Create vertex buffer via rhi.CreateCommittedBuffer(size, UPLOAD, GENERIC_READ)
    2. Map, memcpy vertices, Unmap
    3. Create index buffer via rhi.CreateCommittedBuffer(size, UPLOAD, GENERIC_READ)
    4. Map, memcpy indices, Unmap
    5. Fill buffer views with GPU addresses
```

---

### Step -1.5: Create GPUMeshCache (Lazy Upload Manager)

**File:** `Engine/Renderer/Public/GPUMeshCache.h`

| Member | Type | Description |
|--------|------|-------------|
| m_rhi | D3D12Rhi* | For upload operations |
| m_cache | map<const Mesh*, unique_ptr<GPUMesh>> | CPU mesh → GPU mesh |

| Method | Description |
|--------|-------------|
| `GetOrUpload(cpuMesh)` | Return cached or upload new |
| `Clear()` | Release all GPU meshes |

**GetOrUpload Algorithm:**
```
GetOrUpload(cpuMesh):
    key = &cpuMesh
    if cache contains key:
        return cache[key]
    else:
        gpuMesh = new GPUMesh()
        gpuMesh.Upload(rhi, cpuMesh.GetMeshData())
        cache[key] = gpuMesh
        return gpuMesh
```

---

### Step -1.6: Move RHI Ownership to Renderer

**App.h Changes:**
- REMOVE: `m_rhi : unique_ptr<D3D12Rhi>`
- REMOVE: `m_meshFactory.Upload(*m_rhi)` call

**Renderer.h Changes:**
- ADD: `m_rhi : unique_ptr<D3D12Rhi>` (now OWNS it)
- ADD: `m_gpuMeshCache : unique_ptr<GPUMeshCache>`

**Renderer Constructor:**
```
Renderer(window, assetSystem, ...):
    m_rhi = make_unique<D3D12Rhi>(window.GetHwnd())
    m_gpuMeshCache = make_unique<GPUMeshCache>(*m_rhi)
    // ... rest of initialization
```

---

### Step -1.7: Update Render Loop

**Before (Coupled):**
```
for mesh in scene.GetMeshes():
    mesh.Bind(cmdList)           ← GameFramework code touching cmdList
    DrawIndexedInstanced(...)
```

**After (Decoupled):**
```
for mesh in scene.GetMeshes():
    gpuMesh = m_gpuMeshCache.GetOrUpload(mesh)    ← Renderer handles upload
    gpuMesh.Bind(cmdList)                          ← Renderer owns Bind
    DrawIndexedInstanced(gpuMesh.GetIndexCount())
```

---

### Phase -1 Gate Check

**Compile-Time Verification:**
```
□ grep "D3D12" GameFramework/Public/**/*.h  → No matches
□ grep "Upload|Bind" Mesh.h                 → No matches
□ grep "D3D12Rhi" App.h                     → No matches
```

**Runtime Verification:**
```
□ Application starts without crash
□ Box scene renders identically to before
□ Camera WASD + mouse works
□ ImGui overlay renders
□ Window resize works
□ No D3D12 validation errors
```

**⚠️ DO NOT PROCEED TO PHASE 0 UNTIL BOX SCENE RENDERS CORRECTLY**

---

## Phase 0: RenderContext API

> **Goal:** Create a high-level command abstraction inspired by Frostbite, hiding D3D12 details from render passes.

### Why RenderContext?

| Without RenderContext | With RenderContext |
|----------------------|-------------------|
| Passes call raw D3D12 commands | Passes call semantic methods |
| Barrier management in every pass | Barriers centralized in context |
| Hard to add Vulkan later | Clean abstraction boundary |
| GPU debugging scattered | Single point for validation |

### Step 0.1: Create ResourceState Enum

**File:** `Engine/Renderer/Public/FrameGraph/ResourceState.h`

| State | D3D12 Equivalent | Usage |
|-------|------------------|-------|
| Common | COMMON | Initial/final state |
| RenderTarget | RENDER_TARGET | Writing to color buffer |
| DepthWrite | DEPTH_WRITE | Writing to depth buffer |
| DepthRead | DEPTH_READ | Sampling depth buffer |
| ShaderResource | PIXEL_SHADER_RESOURCE | Sampling in shader |
| UnorderedAccess | UNORDERED_ACCESS | Compute read/write |
| CopySource | COPY_SOURCE | Copy from |
| CopyDest | COPY_DEST | Copy to |
| Present | PRESENT | Swap chain present |

---

### Step 0.2: Create RenderContext Interface

**File:** `Engine/Renderer/Public/RenderContext.h`

**RenderContext** wraps the command list and provides semantic operations:

| Method Category | Methods |
|-----------------|---------|
| **Pipeline** | `SetPipelineState(pso)`, `SetRootSignature(sig)` |
| **Geometry** | `SetPrimitiveTopology(topology)`, `BindVertexBuffer(view)`, `BindIndexBuffer(view)` |
| **Constants** | `BindConstantBuffer(slot, gpuAddress)`, `BindDescriptorTable(slot, handle)` |
| **Render Targets** | `SetRenderTarget(rtv, dsv)`, `ClearRenderTarget(rtv, color)`, `ClearDepthStencil(dsv, depth)` |
| **Viewport** | `SetViewport(x, y, w, h)`, `SetScissorRect(x, y, w, h)` |
| **Draw** | `DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance)` |
| **Barriers** | `TransitionResource(resource, before, after)` |
| **Access** | `GetNativeCommandList()` — escape hatch for UI, etc. |

---

### Step 0.3: Implement D3D12RenderContext

**File:** `Engine/Renderer/Private/D3D12RenderContext.cpp`

**Constructor takes:**
- `ID3D12GraphicsCommandList*` — the active command list

**Each method translates to D3D12:**

| RenderContext Method | D3D12 Translation |
|---------------------|-------------------|
| `SetPipelineState(pso)` | `cmdList->SetPipelineState(pso)` |
| `SetRootSignature(sig)` | `cmdList->SetGraphicsRootSignature(sig)` |
| `BindConstantBuffer(slot, addr)` | `cmdList->SetGraphicsRootConstantBufferView(slot, addr)` |
| `BindDescriptorTable(slot, hdl)` | `cmdList->SetGraphicsRootDescriptorTable(slot, hdl)` |
| `SetRenderTarget(rtv, dsv)` | `cmdList->OMSetRenderTargets(1, &rtv, false, &dsv)` |
| `ClearRenderTarget(rtv, color)` | `cmdList->ClearRenderTargetView(rtv, color, 0, nullptr)` |
| `DrawIndexedInstanced(...)` | `cmdList->DrawIndexedInstanced(...)` |
| `TransitionResource(...)` | Build `D3D12_RESOURCE_BARRIER` + `ResourceBarrier()` |

**TransitionResource maps ResourceState to D3D12:**
```
TransitionResource(resource, before, after):
    barrier = D3D12_RESOURCE_BARRIER
    barrier.Type = TRANSITION
    barrier.Resource = resource
    barrier.StateBefore = MapToD3D12State(before)
    barrier.StateAfter = MapToD3D12State(after)
    cmdList.ResourceBarrier(1, &barrier)
```

---

## Phase 1: Frame Graph Foundation

> **Goal:** Create the core Frame Graph infrastructure — passes declare resources, graph orchestrates execution.

### Step 1.1: Create ResourceHandle

**File:** `Engine/Renderer/Public/FrameGraph/ResourceHandle.h`

| Member | Type | Description |
|--------|------|-------------|
| index | uint32 | Resource slot ID |

| Constant | Value | Meaning |
|----------|-------|---------|
| INVALID | UINT32_MAX | No resource |
| BACKBUFFER | 0 | Swap chain back buffer (MVP) |
| DEPTH_BUFFER | 1 | Depth stencil buffer (MVP) |

---

### Step 1.2: Create RenderPass Base Class

**File:** `Engine/Renderer/Public/FrameGraph/RenderPass.h`

| Method | Parameters | Description |
|--------|------------|-------------|
| Constructor | name | Store pass name for debugging |
| `Setup` (pure virtual) | builder, sceneView | Declare resource dependencies |
| `Execute` (pure virtual) | context | Record GPU commands |
| `GetName` | — | Return pass name |

**Setup vs Execute:**

| Aspect | Setup | Execute |
|--------|-------|---------|
| When called | Before rendering | During rendering |
| Purpose | Declare what resources needed | Record GPU commands |
| GPU work | NONE | All drawing here |
| Receives | PassBuilder, SceneView | RenderContext |

---

### Step 1.3: Create PassBuilder

**File:** `Engine/Renderer/Public/FrameGraph/PassBuilder.h`

| Method | Returns | Description |
|--------|---------|-------------|
| `UseBackBuffer()` | ResourceHandle | Declare write to back buffer |
| `UseDepthBuffer()` | ResourceHandle | Declare write to depth buffer |
| `Read(handle, state)` | ResourceHandle | Declare read (future) |
| `Write(handle, state)` | ResourceHandle | Declare write (future) |
| `CreateTexture(desc)` | ResourceHandle | Create transient (future) |

**MVP Implementation:** Only `UseBackBuffer()` and `UseDepthBuffer()` needed.

---

### Step 1.4: Create FrameGraph Class

**File:** `Engine/Renderer/Public/FrameGraph/FrameGraph.h`

| Member | Type | Description |
|--------|------|-------------|
| m_passes | vector<unique_ptr<RenderPass>> | All registered passes |
| m_swapChain | D3D12SwapChain* | For back buffer access |
| m_depthStencil | D3D12DepthStencil* | For depth buffer access |

| Method | Description |
|--------|-------------|
| `AddPass<T>(name, ...)` | Create and register a pass, return reference |
| `Setup(sceneView)` | Call Setup on all passes |
| `Compile()` | MVP: no-op. Future: barriers, ordering |
| `Execute(context)` | Call Execute on all passes |

**Per-Frame Flow:**
```
1. SceneView view = scene.ExtractSceneView(w, h)
2. frameGraph.Setup(view)      ← All passes declare resources
3. frameGraph.Compile()        ← MVP: no-op
4. frameGraph.Execute(context) ← All passes record commands
```

---

## Phase 2: SceneView Extraction

> **Goal:** Create a pure-data struct that crosses the GameFramework → Renderer boundary.

### Step 2.1: Create SceneView Structure

**File:** `Engine/Renderer/Public/SceneView.h`

**Camera Data:**
| Field | Type | Description |
|-------|------|-------------|
| viewMatrix | float4x4 | Camera view matrix |
| projMatrix | float4x4 | Camera projection matrix |
| viewProjMatrix | float4x4 | Combined for shaders |
| cameraPosition | float3 | World position |
| cameraDirection | float3 | Forward vector |
| nearZ, farZ | float | Clip planes |

**Viewport:**
| Field | Type | Description |
|-------|------|-------------|
| width, height | uint32 | Viewport dimensions |

**Lighting:**
| Field | Type | Description |
|-------|------|-------------|
| sunLight | DirectionalLight | Sun direction, color, intensity |

**DirectionalLight Structure:**
| Field | Type |
|-------|------|
| direction | float3 |
| intensity | float |
| color | float3 |

**Draw Commands:**
| Field | Type | Description |
|-------|------|-------------|
| meshDraws | vector<MeshDraw> | One per mesh instance |
| materials | vector<MaterialData> | All materials |

**MeshDraw Structure:**
| Field | Type | Description |
|-------|------|-------------|
| worldMatrix | float4x4 | Object transform |
| worldInvTranspose | float3x4 | For normals |
| materialId | uint32 | Index into materials[] |
| meshPtr | const void* | Opaque pointer for GPUMeshCache |

**MaterialData Structure:**
| Field | Type |
|-------|------|
| baseColor | float4 |
| metallic | float |
| roughness | float |
| f0 | float |
| albedoTextureIdx | uint32 |

**Key Constraints:**
- ❌ NO D3D12 types
- ❌ NO GPU handles
- ✅ Pure data — can be serialized, logged, replayed

---

### Step 2.2: Implement Scene::ExtractSceneView()

**File:** `Engine/GameFramework/Private/Scene/Scene.cpp`

**Algorithm:**
```
ExtractSceneView(viewportWidth, viewportHeight):
    view = SceneView()
    
    // 1. Camera
    view.viewMatrix = m_camera.GetViewMatrix()
    view.projMatrix = m_camera.GetProjectionMatrix(width/height)
    view.viewProjMatrix = view.viewMatrix * view.projMatrix
    view.cameraPosition = m_camera.GetPosition()
    view.cameraDirection = m_camera.GetForward()
    view.nearZ = m_camera.GetNearZ()
    view.farZ = m_camera.GetFarZ()
    
    // 2. Viewport
    view.width = viewportWidth
    view.height = viewportHeight
    
    // 3. Lighting
    view.sunLight.direction = m_sunDirection  // or default
    view.sunLight.intensity = m_sunIntensity
    view.sunLight.color = m_sunColor
    
    // 4. Materials
    for mat in m_materials:
        view.materials.push_back(mat)
    
    // 5. Mesh draws
    for mesh in m_meshFactory.GetMeshes():
        world = mesh.GetWorldMatrix()
        draw = MeshDraw()
        draw.worldMatrix = world
        draw.worldInvTranspose = transpose(inverse(world))
        draw.materialId = mesh.GetMaterialId()
        draw.meshPtr = &mesh    // Opaque pointer
        view.meshDraws.push_back(draw)
    
    return view
```

---

## Phase 3: glTF Loading

> **Goal:** Load Sponza glTF file into CPU-side MeshData and MaterialDesc.

### Step 3.1: Create GltfLoader

**File:** `Engine/GameFramework/Public/Assets/GltfLoader.h`

**LoadResult Structure:**
| Field | Type | Description |
|-------|------|-------------|
| meshes | vector<MeshData> | Geometry per primitive |
| materials | vector<MaterialDesc> | PBR material parameters |
| texturePaths | vector<string> | Paths to texture files |
| transforms | vector<float4x4> | World transforms |
| success | bool | Load succeeded |
| errorMessage | string | Error details if failed |

**MaterialDesc Structure:**
| Field | Type |
|-------|------|
| name | string |
| baseColor | float4 |
| metallic | float |
| roughness | float |
| f0 | float |
| albedoTexture | optional<path> |
| normalTexture | optional<path> |
| metallicRoughnessTexture | optional<path> |

**Load Algorithm (using cgltf):**
```
Load(filePath):
    result = LoadResult()
    
    // Parse glTF
    data = cgltf_parse_file(filePath)
    cgltf_load_buffers(data, filePath)
    
    // Load materials
    for mat in data.materials:
        desc = MaterialDesc()
        desc.name = mat.name
        if mat.has_pbr_metallic_roughness:
            pbr = mat.pbr_metallic_roughness
            desc.baseColor = pbr.base_color_factor
            desc.metallic = pbr.metallic_factor
            desc.roughness = pbr.roughness_factor
            if pbr.base_color_texture:
                desc.albedoTexture = GetImagePath(...)
        result.materials.push_back(desc)
    
    // Load meshes (each primitive = one MeshData)
    for node in data.nodes:
        if node.mesh == null: continue
        worldTransform = ComputeNodeWorldTransform(node)
        
        for primitive in node.mesh.primitives:
            meshData = MeshData()
            
            // Read accessors
            positions = GetAccessorData(POSITION)
            normals = GetAccessorData(NORMAL)
            texcoords = GetAccessorData(TEXCOORD_0)
            tangents = GetAccessorData(TANGENT)
            indices = GetAccessorData(primitive.indices)
            
            // Build vertices
            for i in 0..positions.count:
                vertex = VertexData()
                vertex.position = positions[i]
                vertex.normal = normals[i] or default
                vertex.uv = texcoords[i] or default
                vertex.tangent = tangents[i] or default
                vertex.color = (1,1,1,1)
                meshData.vertices.push_back(vertex)
            
            meshData.indices = indices
            result.meshes.push_back(meshData)
            result.transforms.push_back(worldTransform)
    
    cgltf_free(data)
    result.success = true
    return result
```

**Key:** GltfLoader is CPU-only. No GPU operations. Scene stores results, Renderer uploads on demand.

---

## Phase 4: ForwardOpaquePass

> **Goal:** Create the concrete render pass that draws all opaque geometry.

### Step 4.1: Create ForwardOpaquePass Class

**File:** `Engine/Renderer/Public/Passes/ForwardOpaquePass.h`

**Constructor Dependencies (injected from Renderer):**
| Dependency | Type | Purpose |
|------------|------|---------|
| m_rhi | D3D12Rhi* | Device access |
| m_rootSignature | D3D12RootSignature* | Pipeline binding |
| m_pso | D3D12PipelineState* | Pipeline state |
| m_cbManager | D3D12ConstantBufferManager* | Constant buffers |
| m_heapManager | D3D12DescriptorHeapManager* | Descriptors |
| m_textureManager | TextureManager* | Texture binding |
| m_gpuMeshCache | GPUMeshCache* | Mesh upload |

**Per-Frame State (set in Setup):**
| State | Type | Purpose |
|-------|------|---------|
| m_sceneView | const SceneView* | Current frame data |
| m_backBuffer | ResourceHandle | Render target |
| m_depthBuffer | ResourceHandle | Depth target |

---

### Step 4.2: Implement Setup Method

```
Setup(builder, sceneView):
    // Capture scene view for Execute
    m_sceneView = &sceneView
    
    // Declare resource usage
    m_backBuffer = builder.UseBackBuffer()
    m_depthBuffer = builder.UseDepthBuffer()
```

---

### Step 4.3: Implement Execute Method

```
Execute(context):
    // 1. PIPELINE STATE
    context.SetRootSignature(m_rootSignature)
    context.SetPipelineState(m_pso)
    context.SetPrimitiveTopology(TRIANGLELIST)
    
    // 2. PER-FRAME CONSTANTS (b0)
    perFrame.time = GetElapsedTime()
    perFrame.deltaTime = GetDeltaTime()
    m_cbManager.UpdatePerFrame(perFrame)
    context.BindConstantBuffer(0, m_cbManager.GetPerFrameGPUAddress())
    
    // 3. PER-VIEW CONSTANTS (b1)
    perView.viewMatrix = m_sceneView.viewMatrix
    perView.projMatrix = m_sceneView.projMatrix
    perView.viewProjMatrix = m_sceneView.viewProjMatrix
    perView.cameraPosition = m_sceneView.cameraPosition
    perView.sunDirection = m_sceneView.sunLight.direction
    perView.sunColor = m_sceneView.sunLight.color
    perView.sunIntensity = m_sceneView.sunLight.intensity
    m_cbManager.UpdatePerView(perView)
    context.BindConstantBuffer(1, m_cbManager.GetPerViewGPUAddress())
    
    // 4. SAMPLER TABLE
    context.BindDescriptorTable(SAMPLER_SLOT, m_heapManager.GetSamplerTableStart())
    
    // 5. DRAW EACH MESH
    for draw in m_sceneView.meshDraws:
        // Get or upload GPU mesh
        cpuMesh = (const Mesh*)draw.meshPtr
        gpuMesh = m_gpuMeshCache.GetOrUpload(*cpuMesh)
        
        // Bind geometry
        context.BindVertexBuffer(gpuMesh.GetVertexBufferView())
        context.BindIndexBuffer(gpuMesh.GetIndexBufferView())
        
        // Per-object VS constants (b2)
        perObjectVS.worldMatrix = draw.worldMatrix
        perObjectVS.worldInvTranspose = draw.worldInvTranspose
        m_cbManager.UpdatePerObjectVS(perObjectVS)
        context.BindConstantBuffer(2, m_cbManager.GetPerObjectVSGPUAddress())
        
        // Per-object PS constants (b3)
        material = m_sceneView.materials[draw.materialId]
        perObjectPS.baseColor = material.baseColor
        perObjectPS.metallic = material.metallic
        perObjectPS.roughness = material.roughness
        perObjectPS.f0 = material.f0
        m_cbManager.UpdatePerObjectPS(perObjectPS)
        context.BindConstantBuffer(3, m_cbManager.GetPerObjectPSGPUAddress())
        
        // Bind texture if present
        if material.albedoTextureIdx != UINT32_MAX:
            srvHandle = m_textureManager.GetSRVHandle(material.albedoTextureIdx)
            context.BindDescriptorTable(TEXTURE_SLOT, srvHandle)
        
        // Draw!
        context.DrawIndexedInstanced(
            gpuMesh.GetIndexCount(),
            1, 0, 0, 0
        )
```

---

## Phase 5: Renderer Integration

> **Goal:** Wire up Frame Graph into Renderer::OnRender().

### Step 5.1: Update Renderer Initialization

**Renderer Constructor additions:**
```
// Create RenderContext
m_renderContext = make_unique<D3D12RenderContext>()

// Create Frame Graph
m_frameGraph = make_unique<FrameGraph>(m_swapChain, m_depthStencil)

// Register passes
m_frameGraph.AddPass<ForwardOpaquePass>(
    "ForwardOpaque",
    m_rhi.get(),
    m_rootSignature.get(),
    m_pso.get(),
    m_cbManager.get(),
    m_heapManager.get(),
    m_textureManager.get(),
    m_gpuMeshCache.get()
)
```

---

### Step 5.2: Update OnRender Loop

```
OnRender():
    // 1. BEGIN FRAME
    m_timer.Tick()
    m_rhi.SetCurrentFrameIndex()
    m_rhi.ResetCommandAllocator()
    m_rhi.ResetCommandList()
    
    // 2. EXTRACT SCENE VIEW
    sceneView = m_scene.ExtractSceneView(width, height)
    
    // 3. FRAME GRAPH: SETUP
    m_frameGraph.Setup(sceneView)
    
    // 4. FRAME GRAPH: COMPILE (MVP: no-op)
    m_frameGraph.Compile()
    
    // 5. RENDER TARGET SETUP (outside frame graph for MVP)
    m_swapChain.TransitionToRenderTarget()
    m_depthStencil.TransitionToDepthWrite()
    SetViewportAndScissor()
    m_swapChain.Clear(clearColor)
    m_depthStencil.Clear()
    m_heapManager.SetShaderVisibleHeaps()
    
    // 6. CREATE RENDER CONTEXT FOR THIS FRAME
    m_renderContext.Begin(m_rhi.GetCommandList())
    
    // 7. FRAME GRAPH: EXECUTE
    m_frameGraph.Execute(m_renderContext)
    
    // 8. UI OVERLAY
    m_ui.Render()
    
    // 9. END FRAME
    m_depthStencil.TransitionToDepthRead()
    m_swapChain.TransitionToPresent()
    m_rhi.CloseAndExecuteCommandList()
    m_swapChain.Present()
    m_rhi.SignalAndAdvanceFrame()
```

---

## Phase 6: Lighting & Polish

> **Goal:** Add directional lighting and finalize Sponza rendering.

### Step 6.1: Update PerViewConstantBuffer

Add sun light data to the per-view constant buffer:

| Field | Type | HLSL |
|-------|------|------|
| sunDirection | float3 | `float3 SunDirection` |
| sunIntensity | float | `float SunIntensity` |
| sunColor | float3 | `float3 SunColor` |

---

### Step 6.2: Update Pixel Shader

**ForwardLitPS.hlsl additions:**

```hlsl
cbuffer PerView : register(b1)
{
    // ... existing camera data
    float3 SunDirection;
    float SunIntensity;
    float3 SunColor;
};

float4 main(PSInput input) : SV_TARGET
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(-SunDirection);
    
    float NdotL = saturate(dot(N, L));
    
    float3 diffuse = BaseColor.rgb * NdotL * SunColor * SunIntensity;
    float3 ambient = BaseColor.rgb * 0.1;  // Simple ambient
    
    return float4(diffuse + ambient, BaseColor.a);
}
```

---

## Module Ownership Diagrams

### Final Object Ownership

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              OBJECT OWNERSHIP                                        │
│                                                                                      │
│  ┌───────────────────────────────────────────────────────────────────────────────┐  │
│  │                           APPLICATION                                          │  │
│  │                                                                                │  │
│  │  App (owns):                                                                   │  │
│  │  ├── Window                                                                    │  │
│  │  ├── Scene                                                                     │  │
│  │  │   ├── Camera                                                                │  │
│  │  │   ├── MeshFactory                                                           │  │
│  │  │   │   └── Meshes[] (CPU only — MeshData inside)                             │  │
│  │  │   └── Materials[] (CPU descriptions)                                        │  │
│  │  │                                                                             │  │
│  │  └── Renderer ◄─────────────────────────────────────────────────────────────┐ │  │
│  │      │                                                                       │ │  │
│  │      │  OWNS ALL GPU:                                                        │ │  │
│  │      ├── D3D12Rhi                                                            │ │  │
│  │      ├── RenderContext                                                       │ │  │
│  │      ├── SwapChain                                                           │ │  │
│  │      ├── DepthStencil                                                        │ │  │
│  │      ├── RootSignature, PSO                                                  │ │  │
│  │      ├── DescriptorHeapManager                                               │ │  │
│  │      ├── ConstantBufferManager                                               │ │  │
│  │      ├── TextureManager                                                      │ │  │
│  │      ├── GPUMeshCache ──────────────────────────────────────────────────┐   │ │  │
│  │      │   └── GPUMesh[] (one per CPU Mesh, lazy uploaded)                │   │ │  │
│  │      │                                                                  │   │ │  │
│  │      └── FrameGraph                                                     │   │ │  │
│  │          └── Passes[]                                                   │   │ │  │
│  │              └── ForwardOpaquePass                                      │   │ │  │
│  │                  ├── Uses GPUMeshCache ─────────────────────────────────┘   │ │  │
│  │                  └── Uses RenderContext                                      │ │  │
│  │                                                                              │ │  │
│  └──────────────────────────────────────────────────────────────────────────────┘ │  │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### Module Dependencies

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                          MODULE DEPENDENCIES                                         │
│                                                                                      │
│   ALLOWED:                                     FORBIDDEN:                           │
│   ─────────                                    ──────────                           │
│                                                                                      │
│   ✅ GameFramework → Core                      ❌ GameFramework → RHI               │
│   ✅ RHI → Core                                ❌ GameFramework → Renderer          │
│   ✅ Renderer → Core                           ❌ RHI → GameFramework               │
│   ✅ Renderer → RHI                            ❌ RHI → Renderer                    │
│   ✅ Renderer → GameFramework (via SceneView)  ❌ Passes → Scene directly           │
│   ✅ Renderer → Platform                                                            │
│                                                                                      │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## File Structure After Implementation

```
Engine/
├── GameFramework/
│   ├── Public/
│   │   ├── Scene/
│   │   │   ├── MeshData.h            ← NEW (Phase -1)
│   │   │   ├── Mesh.h                ← MODIFIED (remove D3D12)
│   │   │   ├── MeshFactory.h         ← MODIFIED (remove Upload)
│   │   │   └── Scene.h               ← MODIFIED (add ExtractSceneView)
│   │   └── Assets/
│   │       ├── GltfLoader.h          ← NEW (Phase 3)
│   │       └── MaterialDesc.h        ← NEW (Phase 3)
│   └── Private/
│       ├── Scene/
│       │   └── Scene.cpp             ← MODIFIED (ExtractSceneView impl)
│       └── Assets/
│           └── GltfLoader.cpp        ← NEW (Phase 3)
│
├── Renderer/
│   ├── Public/
│   │   ├── RenderContext.h           ← NEW (Phase 0)
│   │   ├── GPUMesh.h                 ← NEW (Phase -1)
│   │   ├── GPUMeshCache.h            ← NEW (Phase -1)
│   │   ├── SceneView.h               ← NEW (Phase 2)
│   │   ├── Renderer.h                ← MODIFIED (owns RHI, FrameGraph)
│   │   ├── FrameGraph/
│   │   │   ├── ResourceState.h       ← NEW (Phase 0)
│   │   │   ├── ResourceHandle.h      ← NEW (Phase 1)
│   │   │   ├── RenderPass.h          ← NEW (Phase 1)
│   │   │   ├── PassBuilder.h         ← NEW (Phase 1)
│   │   │   └── FrameGraph.h          ← NEW (Phase 1)
│   │   └── Passes/
│   │       └── ForwardOpaquePass.h   ← NEW (Phase 4)
│   └── Private/
│       ├── D3D12RenderContext.cpp    ← NEW (Phase 0)
│       ├── GPUMesh.cpp               ← NEW (Phase -1)
│       ├── GPUMeshCache.cpp          ← NEW (Phase -1)
│       ├── Renderer.cpp              ← MODIFIED (Phase 5)
│       ├── FrameGraph/
│       │   ├── FrameGraph.cpp        ← NEW (Phase 1)
│       │   └── PassBuilder.cpp       ← NEW (Phase 1)
│       └── Passes/
│           └── ForwardOpaquePass.cpp ← NEW (Phase 4)
│
└── Assets/
    └── Meshes/
        └── Sponza/                   ← Place Sponza glTF here
```

---

## Success Criteria

### Phase -1 Gate (Must Pass Before Continuing)
- [ ] `grep -r "D3D12" GameFramework/Public/` → No matches
- [ ] Box scene renders identically to before decoupling
- [ ] Camera and UI work normally

### Final Success (Phase 6 Complete)
- [ ] Sponza mesh loads and renders
- [ ] Camera can fly through scene
- [ ] Directional lighting visible
- [ ] No D3D12 includes in GameFramework
- [ ] Frame Graph executes ForwardOpaquePass
- [ ] SceneView extraction works cleanly
- [ ] No GPU validation layer errors

---

## Learning Resources

### Primary Sources (Must Read)

| Resource | Author | Description |
|----------|--------|-------------|
| [FrameGraph GDC 2017](https://www.gdcvault.com/play/1024612/) | Yuriy O'Donnell (Frostbite) | THE original Frame Graph presentation |
| [GDC 2017 Slides (PDF)](https://www.ea.com/frostbite/news/framegraph-extensible-rendering-architecture-in-frostbite) | EA Frostbite | Presentation slides |
| [Render Graphs Deep Dive](https://themaister.net/blog/2017/08/15/render-graphs-and-vulkan-a-deep-dive/) | Themaister | Implementation details |

### AMD GPUOpen Resources

| Resource | Description |
|----------|-------------|
| [AMD GPUOpen Graphics](https://gpuopen.com/learn/graphics/) | Graphics learning hub |
| [Cauldron Framework](https://github.com/GPUOpen-LibrariesAndSDKs/Cauldron) | Reference implementation |

### Beginner-Friendly

| Resource | Author |
|----------|--------|
| [Render Graph 101](https://logins.github.io/graphics/2021/05/31/RenderGraphs.html) | Kostas Anagnostou |
| [Simple Render Graph](https://blog.traverseresearch.nl/render-graph-101-f42646255636) | Traverse Research |

### Recommended Learning Order
1. GDC 2017 Slides (skim for big picture)
2. Render Graph 101 blog post
3. Themaister's Deep Dive
4. Study Cauldron source code
5. GDC 2017 video (full understanding)

---

## Technical Debt Considerations

### Accepted Debt (OK for Sponza MVP)

| Debt | Why Acceptable | Future Fix |
|------|----------------|------------|
| No resource aliasing | Single pass, few resources | Add lifetime tracking |
| Manual barriers | Single pass, predictable | Auto-insert from graph |
| Compile() is no-op | No reordering needed | Topological sort |
| Single forward pass | Sponza is opaque | Add transparent pass |
| GPUMeshCache uses pointer keys | Meshes stable | Add proper mesh IDs |
| RenderContext thin wrapper | Good enough for MVP | Expand for multi-backend |

### Avoided Debt (Critical)

| Avoided | Why | Benefit |
|---------|-----|---------|
| ✅ D3D12 in GameFramework | Was the problem! | Clean separation |
| ✅ Upload() in Mesh | GPU code in game logic | Renderer owns GPU |
| ✅ RHI in App | App shouldn't know GPU | Clean initialization |
| ✅ RHI types in SceneView | Would couple modules | Clean data contract |
| ✅ Passes accessing Scene | Violates FrameGraph | SceneView works |

---

## Quick Reference

### Phase Checklist

```
PHASE -1: DECOUPLING
□ Create MeshData.h
□ Refactor Mesh.h (remove D3D12)
□ Refactor MeshFactory.h (remove Upload)
□ Create GPUMesh
□ Create GPUMeshCache
□ Move RHI to Renderer
□ Update render loop
★ GATE: Box scene renders

PHASE 0: RENDER CONTEXT
□ Create ResourceState enum
□ Create RenderContext interface
□ Implement D3D12RenderContext

PHASE 1: FRAME GRAPH
□ Create ResourceHandle
□ Create RenderPass base
□ Create PassBuilder
□ Create FrameGraph

PHASE 2: SCENEVIEW
□ Create SceneView struct
□ Create MeshDraw, MaterialData
□ Implement Scene::ExtractSceneView()

PHASE 3: GLTF
□ Integrate cgltf
□ Create GltfLoader
□ Create MaterialDesc
□ Test load Sponza

PHASE 4: FORWARD PASS
□ Create ForwardOpaquePass
□ Implement Setup
□ Implement Execute

PHASE 5: INTEGRATION
□ Update Renderer init
□ Update OnRender loop
□ Wire up Frame Graph
★ GATE: Sponza renders (unlit)

PHASE 6: LIGHTING
□ Add sun light to PerViewCB
□ Update ForwardLitPS shader
★ GATE: Sponza renders lit
```

---

*Document Version: 2.0 — Unified and coherent plan for Sponza integration with Frame Graph architecture.*
