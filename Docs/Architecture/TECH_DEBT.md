# Sparkle Engine — Technical Debt Tracker

> **Last Updated:** February 7, 2026
> Generated from integration plan + full engine code audit.

---

## Priority Legend

| Priority | Meaning |
|----------|---------|
| **High** | Blocks features or causes correctness issues |
| **Medium** | Quality/scale issue, should fix before next major feature |
| **Low** | Polish, future-proofing, or minor inefficiency |

---

## Summary

| Category | High | Medium | Low | Total |
|----------|------|--------|-----|-------|
| Architecture | 2 | 4 | 3 | 9 |
| Performance | 1 | 3 | 2 | 6 |
| Feature Gap | 4 | 5 | 3 | 12 |
| Code Quality | 0 | 2 | 3 | 5 |
| Shader | 1 | 1 | 1 | 3 |
| Asset Pipeline | 1 | 1 | 2 | 4 |
| **Total** | **9** | **16** | **14** | **39** |

---

## High Priority

### H1. FrameGraph Compile() Is a No-Op
- **Files:** `Engine/Renderer/Private/FrameGraph/FrameGraph.cpp`, `Engine/Renderer/Public/FrameGraph/FrameGraph.h`
- **Category:** Architecture
- **Issue:** `Compile()` does nothing — no dependency analysis, barrier insertion, or pass reordering. The graph is just a linear pass list.
- **Fix:** Implement topological sort based on `Read()`/`Write()` declarations. Auto-insert `ResourceBarrier` calls between passes.

### H2. ForwardOpaquePass Bypasses FrameGraph for Transitions
- **Files:** `Engine/Renderer/Private/Passes/ForwardOpaquePass.cpp` (L73)
- **Category:** Architecture
- **Issue:** Pass directly calls `m_swapChain->SetRenderTargetState()` and `m_depthStencil->SetWriteState()` instead of letting the FrameGraph drive transitions.
- **Fix:** Move transitions into `FrameGraph::Execute()`, driven by `PassBuilder` resource declarations.

### H3. PassBuilder Read/Write/CreateTexture Are Stubs
- **Files:** `Engine/Renderer/Public/FrameGraph/PassBuilder.h` (L54-76)
- **Category:** Feature Gap
- **Issue:** `Read()`, `Write()`, `CreateTexture()` return input unchanged. FrameGraph cannot track dependencies or create transient resources.
- **Fix:** Accumulate read/write sets per pass. `CreateTexture` should allocate from a transient resource pool.

### H4. TextureManager Only Supports Enum-Based Static Textures
- **Files:** `Engine/Renderer/Public/TextureManager.h` (L16-23)
- **Category:** Feature Gap
- **Issue:** Only `Checker` and `SkyCubemap` slots exist. No dynamic path-based loading for per-material textures from glTF. All Sponza objects render with the checker fallback.
- **Fix:** Add `LoadTextureFromPath()` returning a handle. Implement hash-map cache keyed on path. Support BC7 via DirectXTex.

### H5. Material Textures Never Bound
- **Files:** `Engine/Renderer/Private/Renderer.cpp` (~L255), `Engine/Renderer/Private/Passes/ForwardOpaquePass.cpp` (L108-113)
- **Category:** Asset Pipeline
- **Issue:** glTF materials have `albedoTexture`, `normalTexture`, `metallicRoughnessTexture` paths but they are never loaded or bound. `albedoTextureIdx` stays `UINT32_MAX`. ForwardOpaquePass binds only the checker texture globally.
- **Fix:** Extend `TextureManager`, populate `MaterialData::albedoTextureIdx` in `BuildSceneView()`, bind per-draw in the pass.

### H6. No Mipmap Generation
- **Files:** `Engine/RHI/Private/D3D12/Resources/D3D12Texture.cpp` (L20, L46, L106)
- **Category:** Feature Gap
- **Issue:** Three TODOs. `MipLevels = 1` always. Causes aliasing at distance and wastes bandwidth.
- **Fix:** Integrate DirectXTex. Generate mip chain via compute or CPU. Update `MipLevels` and SRV.

### H7. Single Directional Light Only
- **Files:** `Engine/Assets/Shaders/Lighting/LightEvaluation.hlsli` (L38-42)
- **Category:** Shader
- **Issue:** `GetMainLight()` reads one directional light from PerView CB. No point/spot lights, no light list.
- **Fix:** Add structured buffer of lights. Light loop in `CalculateDirect()`. Forward+ tile/cluster culling later.

### H8. D3D12Rhi Missing WaitForMultipleObjects
- **Files:** `Engine/RHI/Private/D3D12/D3D12Rhi.cpp` (L240)
- **Category:** Performance
- **Issue:** TODO: single fence wait per frame instead of multi-fence. Can cause CPU stalls and incorrect pacing.
- **Fix:** Use `WaitForMultipleObjects` with per-frame fence values for correct N-buffered rendering.

### H9. ResourceHandle Only Has Two Hardcoded Handles
- **Files:** `Engine/Renderer/Public/FrameGraph/ResourceHandle.h` (L17-19)
- **Category:** Feature Gap
- **Issue:** Only `BACKBUFFER` and `DEPTH_BUFFER`. No dynamic allocation for G-buffer, shadow maps, etc.
- **Fix:** Add `ResourceRegistry` managing handle allocation, descriptions, and transient resources.

---

## Medium Priority

### M1. GPUMesh Uses Upload Heap (Not Default Heap)
- **Files:** `Engine/Renderer/Private/GPU/GPUMesh.cpp` (L55, L93)
- **Category:** Performance
- **Issue:** VB/IB created with `D3D12_HEAP_TYPE_UPLOAD`. Slower than VRAM-resident default heap for static geometry.
- **Fix:** Create on `DEFAULT` heap with staging upload buffer + copy. Release upload buffer after copy.

### M2. GPUMeshCache Pointer-Keyed Map (Dangling Pointer Risk)
- **Files:** `Engine/Renderer/Public/GPU/GPUMeshCache.h` (L67)
- **Category:** Code Quality
- **Issue:** `std::unordered_map<const Mesh*, GPUMesh>` — if mesh is destroyed and new one allocated at same address, stale GPU data is returned.
- **Fix:** Use stable mesh ID (monotonic counter or content hash) instead of raw pointer.

### M3. MeshDraw Uses void* for Mesh Reference
- **Files:** `Engine/Renderer/Public/SceneData/MeshDraw.h` (L23)
- **Category:** Code Quality
- **Issue:** `const void* meshPtr` loses type safety. Unchecked `static_cast` in pass.
- **Fix:** Use `const Mesh*` or a typed `MeshHandle` wrapper.

### M4. Synchronous Texture Loading
- **Files:** `Engine/Renderer/Public/TextureManager.h` (L25)
- **Category:** Performance
- **Issue:** All loads block the main thread. Large scenes stall for seconds at startup.
- **Fix:** Async loading with job queue. Use placeholder (1x1 pink) texture until ready.

### M5. Per-Frame SceneView Allocation
- **Files:** `Engine/Renderer/Private/Renderer.cpp` (L181)
- **Category:** Performance
- **Issue:** `BuildSceneView()` allocates `MeshDraw` and `MaterialData` vectors every frame. 800+ Sponza meshes = non-trivial allocs.
- **Fix:** Cache SceneView, rebuild only on dirty flag. Or pre-allocate and memcpy updates.

### M6. Hardcoded Sponza Path in App
- **Files:** `Engine/Application/Private/App.cpp` (L58)
- **Category:** Architecture
- **Issue:** `"Sponza/Sponza.gltf"` hardcoded. No command-line args, project config, or scene selection.
- **Fix:** Add project config file or CLI arg for startup scene.

### M7. Renderer Destructor Order-Sensitive
- **Files:** `Engine/Renderer/Private/Renderer.cpp` (L288-312)
- **Category:** Architecture
- **Issue:** Manual `unique_ptr::reset()` ordering is fragile. Wrong order = GPU use-after-free.
- **Fix:** Document dependency chain. Consider layered ownership or dependency-aware shutdown.

### M8. Depth/Present Transitions Outside FrameGraph
- **Files:** `Engine/Renderer/Private/Renderer.cpp` (L199-200)
- **Category:** Architecture
- **Issue:** End-of-frame transitions done after `Execute()` instead of by the graph.
- **Fix:** Move into FrameGraph or a dedicated present pass.

### M9. Scene GetMeshes() Returns Different Collections
- **Files:** `Engine/GameFramework/Private/Scene/Scene.cpp` (L113-123)
- **Category:** Architecture
- **Issue:** Returns `m_importedMeshes` if non-empty, else `m_meshFactory->GetMeshes()`. Implicit priority makes behavior unpredictable.
- **Fix:** Merge all meshes into unified `m_meshes` vector. Populate from either source at load time.

### M10. No Shadow Mapping
- **Files:** `Engine/Renderer/` (entire module)
- **Category:** Feature Gap
- **Issue:** No shadow pass exists. Sun light has no shadow map.
- **Fix:** Add `ShadowMapPass` rendering depth from sun perspective. Bind shadow map SRV in ForwardOpaquePass.

### M11. No Transparent/Alpha Pass
- **Files:** `Engine/Renderer/Private/Passes/ForwardOpaquePass.cpp`
- **Category:** Feature Gap
- **Issue:** Only opaque geometry rendered. Alpha objects (glass, curtains) drawn opaque.
- **Fix:** Add `ForwardTransparentPass` with back-to-front sorting and alpha blending.

### M12. No BRDF LUT Texture
- **Files:** `Engine/Assets/Shaders/BRDF/Specular.hlsli` (L84)
- **Category:** Shader
- **Issue:** Uses polynomial approximation instead of pre-computed BRDF LUT. LUT path exists but no texture generated. Also typo: "Genenrate".
- **Fix:** Generate 512x512 BRDF integration LUT at startup (compute shader). Bind as global SRV.

### M13. GltfLoader URI Percent-Encoding Not Handled
- **Files:** `Engine/GameFramework/Private/Assets/GltfLoader.cpp` (L103-106)
- **Category:** Asset Pipeline
- **Issue:** Files with spaces or special characters in names fail to load.
- **Fix:** Copy URI string, call `cgltf_decode_uri` on copy, use decoded path.

### M14. No Instanced Rendering
- **Files:** `Engine/RHI/Public/D3D12/Resources/D3D12ConstantBufferData.h` (L112)
- **Category:** Feature Gap
- **Issue:** TODO. Each mesh gets its own draw call with unique CB allocation. No batching.
- **Fix:** Define `PerInstanceData` structured buffer. Batch same-material/mesh draws.

### M15. No Multi-Threaded Command Recording
- **Files:** `Engine/RHI/Private/D3D12/Resources/D3D12ConstantBufferManager.cpp` (L88)
- **Category:** Performance
- **Issue:** Ring allocator is thread-safe but all recording is single-threaded.
- **Fix:** Use command list bundles or deferred lists recorded on worker threads.

### M16. DirectionalLight Has No UI Control
- **Files:** `Engine/Renderer/Public/SceneData/DirectionalLight.h` (L18-22)
- **Category:** Feature Gap
- **Issue:** Direction/color/intensity use compile-time defaults. No editor panel or scene-authored values.
- **Fix:** Add ImGui sliders for sun parameters.

---

## Low Priority

### L1. GPUMesh::Bind Sets Topology Redundantly
- **Files:** `Engine/Renderer/Private/GPU/GPUMesh.cpp` (L139)
- **Issue:** Both `Bind()` and `ForwardOpaquePass::Execute()` set topology. Remove from `Bind()` — topology is a pipeline concern.

### L2. GPUMeshCache No Eviction/Invalidation
- **Files:** `Engine/Renderer/Public/GPU/GPUMeshCache.h`
- **Issue:** Cache grows unboundedly, no LRU eviction, no invalidation on mesh destruction.
- **Fix:** Add `Invalidate(const Mesh&)`. Consider `Clear()` on scene transition.

### L3. ImportedMesh Inverse-Transpose Computed Every Frame
- **Files:** `Engine/GameFramework/Private/Scene/ImportedMesh.cpp` (L21-25)
- **Issue:** Static mesh recomputes `XMMatrixInverse` per frame. Should cache at construction.

### L4. ImportedMesh GenerateGeometry Deep-Copies Data
- **Files:** `Engine/GameFramework/Private/Scene/ImportedMesh.cpp` (L31-35)
- **Issue:** Copies vertex/index arrays unnecessarily. Should provide direct const access.

### L5. GltfLoader No Vertex Color Loading
- **Files:** `Engine/GameFramework/Private/Assets/GltfLoader.cpp` (L239)
- **Issue:** `COLOR_0` attribute ignored. Vertex color always white.

### L6. GltfLoader Hardcoded Max Node Depth
- **Files:** `Engine/GameFramework/Private/Assets/GltfLoader.cpp` (L78)
- **Issue:** `cgltf_node* chain[64]` — 64-deep limit. Silently wrong for deeper hierarchies.

### L7. GltfLoader Linear Search for Texture Dedup
- **Files:** `Engine/GameFramework/Private/Assets/GltfLoader.cpp` (L148)
- **Issue:** `std::ranges::find` is O(n). Use `std::unordered_set` for O(1) dedup.

### L8. GltfLoader Only Triangles Supported
- **Files:** `Engine/GameFramework/Private/Assets/GltfLoader.cpp` (L363)
- **Issue:** Non-triangle primitives silently skipped. At least log a warning.

### L9. Scene Static Empty Vector
- **Files:** `Engine/GameFramework/Private/Scene/Scene.cpp` (L118)
- **Issue:** Function-local static of complex type. Minor code smell.

### L10. ResourceState Maps ShaderResource Only to Pixel Shader
- **Files:** `Engine/Renderer/Private/RenderContext.cpp` (L170)
- **Issue:** No `NonPixelShaderResource` state. Breaks compute SRV access.

### L11. RenderContext Is MVP Thin Wrapper
- **Files:** `Engine/Renderer/Public/RenderContext.h` (L22)
- **Issue:** 1:1 D3D12 mapping. Acceptable now, needs abstraction for Vulkan/Metal.

### L12. ConstantBuffer 256-Byte Slot Padding Waste
- **Files:** `Engine/Assets/Shaders/Resources/ConstantBuffers.hlsli` (L56)
- **Issue:** PerObjectVS/PS waste ~60% of 256-byte slots. Switch to structured buffer later.

### L13. ForwardOpaquePass Clears Inside Pass
- **Files:** `Engine/Renderer/Private/Passes/ForwardOpaquePass.cpp` (L96-97)
- **Issue:** `Clear()` calls embedded in pass, not driven by FrameGraph. Reordering passes could clear wrong buffer.

### L14. PerView CB Has Reserved Unused Space
- **Files:** `Engine/RHI/Public/D3D12/Resources/D3D12ConstantBufferData.h` (L91)
- **Issue:** Reserved padding. Intentional future-proofing, no action needed yet.

---

## Future Feature Wishlist (From Integration Plan Non-Goals)

These were explicitly out of scope for Sponza MVP:

| Feature | Notes |
|---------|-------|
| Deferred rendering / G-Buffer | Requires multiple render targets, geometry pass |
| Shadow maps | See M10 above |
| Resource aliasing & automatic barriers | See H1, H2, H3 |
| Multiple render passes (transparency, post-processing) | See M11 |
| Async compute | Requires concurrent command queues |
| Texture streaming | See H4 |
| Multi-backend (Vulkan/Metal) | See L11 |

---

*This document should be updated as items are resolved or new debt is identified.*
