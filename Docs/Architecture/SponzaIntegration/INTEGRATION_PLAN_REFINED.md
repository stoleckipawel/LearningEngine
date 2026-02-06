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
    float3 SunColor;s
};



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
