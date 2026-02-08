<!-- ========================================================================= -->
<!-- TEXTURE PIPELINE DESIGN - glTF to RHI                                    -->
<!-- Sparkle Engine - Architecture Decision Record                             -->
<!-- ========================================================================= -->

# Texture Pipeline Design - glTF to RHI

**Author:** Engine Architecture
**Status:** Draft
**Date:** 2026-02-08

---

## 1. Problem Statement

Sponza materials reference multiple textures (albedo, normal, metallic-roughness, etc.), but the current renderer binds a single default checker texture for all draws. The pipeline extracts texture paths from glTF but does not transport them into GPU resources or bind them per material.

Goals:
- Preserve Level -> Scene -> Renderer separation
- Support per-material texture binding with correct fallbacks
- Keep system complexity similar to existing engine modules (simple, explicit)
- Provide a clean path to future extensions (more maps, streaming, bindless)
- Support mipmap generation and texture compression in the load pipeline

Non-goals (for this phase):
- Texture streaming, virtual texturing
- Async loading and background uploads

---

## 2. Current State (Summary)

Pipeline today:

```
GltfLoader::ExtractMaterials
  -> MaterialDesc has texture paths (albedo/normal/metallicRoughness)
  -> LoadResult.texturePaths filled

Scene::AppendGltf
  -> stores MaterialDesc in Scene

Renderer::BuildMaterials
  -> MaterialData::FromDesc ignores texture paths

ForwardOpaquePass
  -> binds checker texture once at t0
```

Key gaps:
- TextureManager only supports fixed enum slots (Checker, Sky)
- MaterialData never references texture handles
- Root signature allows only one SRV (t0)
- Shader samples t0 for all materials

---

## 3. Design Options

### Option A: Per-Draw SRV Binding (Baseline)

**Description:**
- Add a path-based texture cache in TextureManager
- MaterialData stores a GPU SRV handle (or a small index) for each texture
- ForwardOpaquePass binds the correct SRV per draw call
- Shader uses texture when available, otherwise uses scalar base color

**Pros:**
- Simple and explicit; matches current engine style
- Minimal shader and root signature changes
- Works with current SRV table size (t0 only)
- Easy to reason about and debug

**Cons:**
- Per-draw SRV binding cost (acceptable for this scale)
- No batching by material (future optimization needed)

**Texture loading and mips:**
- Use DirectXTex to load images and generate mip chains
- Create SRVs with full mip chains
- Add a compression step (BC7 for color, BC5 for normals) when ready

**Industry references:**
- UE4 forward renderer historically used per-draw material binding
- Frostbite and id Tech use similar explicit binding in non-bindless paths

**Scalability path:**
- Add material sorting to reduce rebinding
- Extend MaterialData with additional maps (normal, MR, AO)

---

### Option B: Per-Material Descriptor Table (Small Bindless)

**Description:**
- Allocate a contiguous SRV range per material
- Each material owns a descriptor table with N textures
- Per-draw, bind only the table handle once
- Shader samples by fixed slot (t0 albedo, t1 normal, t2 MR)

**Pros:**
- Fewer descriptor binds than Option A
- Clear material boundary, closer to modern engines

**Cons:**
- Requires new descriptor allocation strategy
- Material lifetime and descriptor lifetime must be managed carefully

**Industry references:**
- UE4 uses material uniform buffers plus SRV tables per material
- Many D3D12 engines use per-material descriptor tables before bindless

**Scalability path:**
- Swap SRV tables to bindless indices when ready

---

### Option C: Global Bindless Texture Array

**Description:**
- Maintain a global texture heap
- MaterialData stores integer indices into the heap
- Shader uses indices to sample from a descriptor array

**Pros:**
- Minimal per-draw binding
- Scales to many textures and materials

**Cons:**
- Higher complexity (descriptor heap management, indexing)
- Requires SM 6.6 or root signature changes for dynamic indexing

**Industry references:**
- UE5, RE Engine, and modern AAA engines use bindless or virtualized texture systems

**Scalability path:**
- Long term goal once core correctness is proven

---

## 4. Decision

**Recommended:** Option A now, Option B as the next evolution, Option C long term.

Rationale:
- Option A requires the smallest change set and matches the engine's current level of abstraction
- It establishes correct data transport without locking in advanced complexity
- It keeps the pipeline close to existing systems (explicit, non-virtualized)

---

## 5. Proposed Architecture (Option A)

### Data Flow

```
LevelDesc
  -> Scene owns MaterialDesc (paths)
  -> Renderer::BuildMaterials
        MaterialData { scalars + albedoSRV }
  -> ForwardOpaquePass
        bind material SRV per draw
  -> Shader
        sample albedo or fallback
```

### Key Types

- **MaterialDesc** (GameFramework)
  - baseColor, metallic, roughness, f0
  - optional paths: albedo, normal, metallicRoughness

- **MaterialData** (Renderer)
  - scalars (baseColor, metallic, roughness, f0)
  - GPU SRV handle for albedo
  - boolean flag for texture presence

- **TextureManager** (Renderer)
  - path-based cache: map<path, D3D12Texture>
  - default texture fallback

---

## 6. Implementation Plan (Option A)

1. **TextureManager path cache**
   - Add LoadFromPath(path) -> D3D12Texture*
   - Cache by absolute path string
   - Keep existing enum slots for defaults

2. **DirectXTex-based load + mip generation**
  - Load WIC/TGA/DDS with DirectXTex
  - Generate mip chain (GenerateMipMaps)
  - Upload all mip levels to GPU
  - Add compression step later (BC7 for albedo, BC5 for normals)

3. **MaterialData::FromDesc**
   - Accept TextureManager reference
   - If desc.albedoTexture exists: load and store GPU SRV handle
   - Add HasAlbedoTexture flag

4. **PerObjectPS constant buffer**
   - Add HasAlbedoTexture to CB data (C++ and HLSL)

5. **ForwardOpaquePass**
   - Bind texture SRV per draw based on MaterialData
   - Fallback to default checker if not set

6. **Shader sampling**
   - Use texture when HasAlbedoTexture == true, else BaseColor

---

## 7. Future Extensions

Short term:
- Add normal map and metallic-roughness SRVs
- Expand root signature SRV range to t0..t2
- Extend MaterialData to store multiple handles
- Add BC compression path for released builds

Medium term:
- Per-material descriptor tables (Option B)
- Material sorting to reduce SRV binds

Long term:
- Bindless texture arrays
- Streaming and mip generation
- Texture compression pipeline (BC7/BC5)

---

## 8. Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Per-draw SRV binds cost | Low | Material sorting later |
| Incorrect path resolution | Medium | Centralize in TextureManager using AssetSystem paths |
| Missing textures in glTF | Low | Default checker fallback |

---

## 9. References

- Unreal Engine: material system with uniform buffers and SRV tables (historical UE4), bindless in UE5
- Frostbite: explicit resource binding with material data staging
- id Tech: explicit material binding, later moved to bindless in recent iterations

---

## 10. Decision Record

- **Adopt Option A** for immediate correctness and minimal complexity.
- **Plan Option B** as next step once correctness is proven.
- **Reserve Option C** for future scale when material count and texture variety require it.
