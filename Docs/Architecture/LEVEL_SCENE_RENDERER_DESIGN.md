<!-- ========================================================================= -->
<!-- LEVEL → SCENE → RENDERER DATA TRANSPORT — Design Review                  -->
<!-- Sparkle Engine — Architecture Decision Record                            -->
<!-- ========================================================================= -->

# Level → Scene → Renderer: Data Transport Design

**Author:** Engine Architecture  
**Status:** Proposed  
**Date:** 2026-02-07  

---

## 1. Problem Statement

Three subsystems must collaborate to go from **authored content** to **pixels on screen**:

| Layer | Module | Responsibility |
|-------|--------|----------------|
| **Level** | GameFramework | Declares *what* should exist (assets, entities, camera, lights) |
| **Scene** | GameFramework | Runtime container that *owns* game objects |
| **Renderer** | Renderer | Extracts a *render-ready snapshot* and draws it |

The central question: **How does data flow between these layers, and where do ownership, loading, and transformation responsibilities live?**

The design must satisfy:
- **Separation of concerns** — Level knows nothing about GPU; Renderer knows nothing about asset files
- **Extensibility** — new asset types, new level types, new render passes
- **Testability** — each layer independently testable
- **Performance** — minimize per-frame allocations and redundant copies
- **Real-engine alignment** — patterns used by UE5, Frostbite, DOOM engine

---

## 2. Current Architecture (Status Quo)

```
Level::OnLoad(LevelContext{scene, assetSystem})
  │
  ├── SponzaLevel calls scene.LoadGltf(path)     ← Level drives loading
  ├── BasicShapesLevel calls scene.SetPrimitives  ← Level drives loading
  │
  ▼
Scene owns:
  ├── GameCamera
  ├── vector<unique_ptr<Mesh>> (imported or procedural)
  ├── vector<MaterialDesc>  (CPU PBR descriptions)
  └── MeshFactory (procedural)
  │
  ▼
Renderer::BuildSceneView() [per frame]
  ├── Converts MaterialDesc → MaterialData (Renderer-side)
  ├── Iterates Scene::GetMeshes() → emits MeshDraw per mesh
  └── Returns SceneView { camera, meshDraws, materials, lights }
  │
  ▼
ForwardOpaquePass::Execute(RenderContext)
  └── Draws meshDraws with GPU resources from GPUMeshCache
```

### Problems with Current Design

| # | Issue | Severity |
|---|-------|----------|
| P1 | **Level performs loading** — `SponzaLevel::OnLoad` calls `scene.LoadGltf()`. Level is both *data definition* and *imperative loader*. Violates SRP. | High |
| P2 | **MaterialDesc → MaterialData conversion happens every frame** — `BuildSceneView()` re-converts the entire material array each frame even when nothing changed. | Medium |
| P3 | **No declarative level description** — Level must know Scene's API. Adding a new content type (e.g., lights, volumes) requires changing Level and Scene simultaneously. | High |
| P4 | **No diff/change tracking** — Renderer re-extracts everything every frame. No dirty flags or change sets. | Medium |
| P5 | **Mixed concerns in Scene** — Scene owns camera, meshes, materials, mesh factory, AND level state. Acts as god object. | Medium |

---

## 3. Design Options

### Option A: Imperative Callback (Current)

```cpp
class Level {
    virtual void OnLoad(LevelContext& ctx) = 0;  // Level calls scene APIs
};
```

**How it works:** Level receives a `LevelContext` with references to Scene and AssetSystem. Level imperatively calls `scene.LoadGltf()`, `scene.SetPrimitives()`, etc.

| Pros | Cons |
|------|------|
| Simple to implement and understand | Level coupled to Scene's full API surface |
| Maximum flexibility — level can do anything | Not serializable / not data-driven |
| No intermediate data structures needed | Hard to diff, preview, or validate without loading |
| Easy for prototype/learning engine | Level is logic, not data — can't be authored by tools |
| | Adding content types requires Level API changes |

**Used by:** Early UE3-era Kismet, many hobby engines

**Verdict:** Acceptable for a learning engine, but architecturally a dead-end for production.

---

### Option B: Declarative Descriptor (Proposed)

```cpp
struct LevelDesc {
    Level::CameraSetup camera;
    vector<MeshRequest> meshes;           // What to load
    vector<MaterialDesc> materials;       // Already have this type
    vector<LightDesc> lights;             // Future
    PrimitiveConfig proceduralPrimitives; // Optional fallback
};

class Level {
    virtual LevelDesc BuildDescription() const = 0;  // Level returns data
};

// Scene processes it:
void Scene::LoadLevel(const LevelDesc& desc, AssetSystem& assets);
```

**How it works:** Level produces a pure-data `LevelDesc`. Scene consumes it, performs all loading, and owns the results. Level never touches Scene directly.

| Pros | Cons |
|------|------|
| Level is pure data — serializable, diffable, tool-authorable | Requires defining all content types up-front in LevelDesc |
| Scene is the single loader — clear ownership | Less flexible than imperative for exotic setups |
| Easy to validate, preview, log | LevelDesc struct grows as engine features grow |
| Matches UE5 World Partition / Frostbite SubLevel data model | Slightly more boilerplate per content type |
| Decoupled — Level doesn't include Scene.h | |
| Testable — can unit test LevelDesc without Scene | |

**Used by:** UE5 (FWorldPartitionActorDesc), Frostbite (SubLevel blueprints), Unity (SceneAsset)

**Verdict:** Best balance of engineering rigor and learning engine scope.

---

### Option C: Entity-Component with Archetype Registry

```cpp
struct LevelDesc {
    vector<EntityDesc> entities;  // Each has components
};

struct EntityDesc {
    TransformComponent transform;
    optional<MeshComponent> mesh;
    optional<LightComponent> light;
    optional<CameraComponent> camera;
};
```

**How it works:** Full ECS-style entity descriptions. Level defines a list of entities, each with optional components. Scene instantiates entities from descriptors.

| Pros | Cons |
|------|------|
| Maximum extensibility — any component, any entity | Massive over-engineering for current engine state |
| Matches UE5 Actor/Component, Unity GameObject | Requires full ECS or entity framework first |
| Natural path to editor, serialization, networking | Significantly more code and abstraction |
| Industry-standard end goal | Components need registration, reflection, factories |

**Used by:** UE5 (Actor/Component), Unity (GameObject/MonoBehaviour), DOOM Eternal (entity archetypes)

**Verdict:** Correct long-term destination, but premature without ECS infrastructure.

---

### Option D: Command Pattern / Scene Transaction Log

```cpp
class Level {
    virtual vector<SceneCommand> GetCommands() const = 0;
};

// Commands:
struct LoadMeshCommand { filesystem::path path; XMFLOAT4X4 transform; };
struct SpawnPrimitivesCommand { PrimitiveConfig config; };
struct SetCameraCommand { CameraSetup camera; };
struct AddLightCommand { LightDesc light; };
```

**How it works:** Level produces an ordered list of commands. Scene replays them. Commands can be logged, replayed, undone.

| Pros | Cons |
|------|------|
| Undoable, replayable, serializable | Adds command infrastructure / visitor pattern |
| Natural fit for editor undo/redo later | Ordering dependencies between commands |
| Each command is independently testable | More types to maintain than descriptor approach |
| Decoupled — commands are value types | Overkill unless building an editor |

**Used by:** DOOM engine (entity spawn commands), some editor frameworks

**Verdict:** Powerful but over-engineered without an editor. Consider post-editor.

---

## 4. Decision Matrix

| Criterion (weight) | A: Imperative | B: Descriptor | C: ECS | D: Commands |
|--------------------|:---:|:---:|:---:|:---:|
| Separation of concerns (25%) | 2 | 5 | 5 | 4 |
| Implementation effort (20%) | 5 | 4 | 1 | 2 |
| Extensibility (20%) | 2 | 4 | 5 | 4 |
| Testability (15%) | 2 | 5 | 5 | 4 |
| Real-engine alignment (10%) | 1 | 4 | 5 | 3 |
| Performance (10%) | 4 | 4 | 3 | 3 |
| **Weighted Score** | **2.7** | **4.4** | **3.8** | **3.3** |

---

## 5. Proposed Design: Option B — Declarative Descriptor

### 5.1 New Types

```cpp
// ---- GameFramework/Public/Level/LevelDesc.h ----

/// What mesh asset to load and where to place it
struct MeshRequest
{
    std::filesystem::path assetPath;    // Relative path (e.g., "Sponza/Sponza.gltf")
    AssetType assetType = AssetType::Mesh;
};

/// Complete declarative level description
struct LevelDesc
{
    // Camera
    Level::CameraSetup camera;

    // Mesh assets to load
    std::vector<MeshRequest> meshAssets;

    // Procedural primitives (if no mesh assets)
    std::optional<Scene::PrimitiveConfig> primitives;

    // Lighting (future-ready)
    // std::vector<LightDesc> lights;
};
```

### 5.2 Refactored Level

```cpp
class Level {
    virtual std::string_view GetName() const = 0;
    virtual std::string_view GetDescription() const = 0;
    virtual LevelDesc BuildDescription() const = 0;  // NEW: returns data only
};
```

`OnLoad` / `OnUnload` / `LevelContext` are removed. Level becomes a pure data factory.

### 5.3 Refactored Scene

```cpp
// Scene becomes the sole loader / interpreter
void Scene::LoadLevel(const Level& level, AssetSystem& assetSystem)
{
    Clear();

    LevelDesc desc = level.BuildDescription();

    // Camera
    m_camera->SetPosition(desc.camera.position);
    m_camera->SetYawPitch(desc.camera.yawRadians, desc.camera.pitchRadians);

    // Load mesh assets
    for (const auto& req : desc.meshAssets)
    {
        auto resolved = assetSystem.ResolvePath(req.assetPath, req.assetType);
        if (resolved) LoadGltf(*resolved);
    }

    // Procedural fallback
    if (desc.primitives && m_importedMeshes.empty())
    {
        SetPrimitiveConfig(*desc.primitives);
    }

    m_currentLevelName = std::string(level.GetName());
}
```

### 5.4 Example: SponzaLevel (Before → After)

**Before (Imperative):**
```cpp
void SponzaLevel::OnLoad(LevelContext& context)
{
    auto path = context.assetSystem.ResolvePath("Sponza/Sponza.gltf", AssetType::Mesh);
    if (path) context.scene.LoadGltf(*path);
}
```

**After (Declarative):**
```cpp
LevelDesc SponzaLevel::BuildDescription() const
{
    LevelDesc desc;
    desc.camera = GetDefaultCamera();
    desc.meshAssets.push_back({"Sponza/Sponza.gltf", AssetType::Mesh});
    return desc;
}
```

The level no longer includes Scene.h. It no longer knows how loading works. It just says *what* it wants.

### 5.5 Benefits Delivered

| Problem | Resolution |
|---------|------------|
| P1: Level performs loading | Level returns data; Scene loads |
| P3: No declarative description | LevelDesc is serializable pure data |
| P4: No change tracking | LevelDesc enables future diffing |
| Interview signal | Shows SRP, data-driven design, layered architecture |

---

## 6. Renderer Data Transport (Scene → SceneView)

This is the second boundary: **GameFramework → Renderer**. Currently `BuildSceneView()` re-extracts every frame (problem P2).

### 6.1 Options

| Approach | Description | Used By |
|----------|-------------|---------|
| **Full extraction (current)** | Rebuild SceneView from scratch each frame | Simple renderers, early prototypes |
| **Dirty-flag extraction** | Scene marks changed meshes/materials; Renderer only updates deltas | Unity DOTS, Godot |
| **Proxy objects** | Renderer maintains persistent render proxies mirroring scene objects | UE5 FPrimitiveSceneProxy |
| **Double-buffered snapshot** | Scene writes to frame N+1 buffer while Renderer reads frame N | Frostbite, DOOM |

### 6.2 Current Recommendation: Full Extraction (Keep for Now)

**Justification:**
- The engine has ~500 meshes maximum currently
- `BuildSceneView()` is < 0.1ms at this scale
- Adding dirty flags or proxy systems now adds complexity without measurable benefit
- Full extraction is **correct** — it's the baseline that more advanced systems optimize

**Future milestone trigger:** When profiling shows `BuildSceneView()` > 1ms, migrate to dirty-flag or proxy model. At that point, `LevelDesc` enables easy diffing.

### 6.3 MaterialDesc → MaterialData Conversion

Current: rebuilt every frame in `BuildSceneView()`.  
Proposed: cache in Scene, invalidate on level load.

```cpp
// Future optimization (not needed yet):
// Scene caches MaterialData[] alongside MaterialDesc[]
// Only rebuilt on LoadLevel/LoadGltf
// Renderer reads cached version directly
```

This is a **measured optimization** — profile first, optimize second.

---

## 7. Data Ownership Summary

```
┌─────────────────────────────────────────────────────────────┐
│                     DATA OWNERSHIP MAP                       │
├─────────────┬────────────────────┬──────────────────────────┤
│ Layer        │ Owns               │ Produces                 │
├─────────────┼────────────────────┼──────────────────────────┤
│ Level        │ (nothing)          │ LevelDesc (value type)   │
│ Scene        │ Camera, Meshes,    │ GetMeshes(),             │
│              │ Materials, Factory │ GetLoadedMaterials()     │
│ Renderer     │ RenderCamera,      │ SceneView (per frame)    │
│              │ GPUMeshCache,      │                          │
│              │ FrameGraph         │                          │
│ RenderPass   │ (nothing)          │ GPU commands             │
└─────────────┴────────────────────┴──────────────────────────┘
```

---

## 8. Migration Path

| Phase | Work | Complexity |
|-------|------|------------|
| **Phase 1** (now) | Introduce `LevelDesc`, refactor 3 built-in levels, remove `OnLoad`/`LevelContext` | Small — ~200 lines changed |
| **Phase 2** (future) | Add `LightDesc` to `LevelDesc`, wire into SceneView | Small |
| **Phase 3** (future) | Scene dirty flags for materials/meshes | Medium |
| **Phase 4** (future) | Entity-Component archetype in LevelDesc (Option C migration) | Large |

---

## 9. Interview Talking Points

This design demonstrates several principles interviewers at NVIDIA/AMD/Epic value:

1. **Data-Oriented Design** — LevelDesc is a POD-like struct. No inheritance, no virtuals for the data itself. Cache-friendly, serializable.

2. **Separation of Concerns** — Level doesn't know about Scene internals. Scene doesn't know about GPU. Renderer doesn't know about files.

3. **Layered Architecture** — Strict dependency direction: Level → LevelDesc → Scene → SceneView → RenderPass. No circular dependencies.

4. **YAGNI + Migration Path** — We don't build ECS now, but the descriptor pattern is a natural stepping stone. Shows pragmatic engineering judgment.

5. **Performance Awareness** — We acknowledge full extraction is O(n) per frame but justify it with measured reasoning, not premature optimization.

6. **Real-Engine Precedent** — UE5's `FWorldPartitionActorDesc`, Frostbite's data-driven SubLevels, Unity's `SceneAsset` all follow the descriptor pattern.
