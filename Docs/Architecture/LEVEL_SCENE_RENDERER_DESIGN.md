<!-- ========================================================================= -->
<!-- LEVEL → SCENE → RENDERER DATA TRANSPORT — Design Review                  -->
<!-- Sparkle Engine — Architecture Decision Record                            -->
<!-- ========================================================================= -->

# Level → Scene → Renderer: Data Transport Design

**Author:** Engine Architecture  
**Status:** In Progress (Phase 1)  
**Date:** 2026-02-07  
**Last Updated:** 2026-02-07  

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

## 2. Current Architecture (as of 2026-02-07)

```
Level::BuildDescription() -> LevelDesc               ← Declarative (Option B)
    │
    ▼
Scene::LoadLevel(level, assetSystem)
    ├── resolves mesh asset paths, calls LoadGltf()
    ├── handles procedural mesh requests via MeshFactory
    │
    ▼
Scene owns:                                          ← IMPROVED: pure container
  ├── GameCamera
  ├── vector<unique_ptr<Mesh>> m_meshes               (unified — no dual-path)
  └── vector<MaterialDesc>     m_loadedMaterials
  │
  ▼
Renderer::BuildSceneView() [per frame]
  ├── BuildMaterials()  — MaterialDesc → MaterialData via FromDesc()
  ├── BuildMeshDraws()  — iterates GetMeshes() → MeshDraw per mesh
  └── Returns SceneView { camera, meshDraws, materials, sunLight }
  │
  ▼
ForwardOpaquePass::Execute(RenderContext)
  └── Draws meshDraws with GPU resources from GPUMeshCache
```

### What Improved Since Initial Design

| # | Change | Impact |
|---|--------|--------|
| ✅ | **Scene is a pure container** — no MeshFactory, no PrimitiveConfig, no RebuildGeometry | P5 resolved — Scene is no longer a god object |
| ✅ | **Unified mesh vector** — single `m_meshes`, no imported-vs-procedural priority logic | Simpler, no ambiguous fallback behavior |
| ✅ | **No CameraSetup in Level** — removed entirely, camera uses GameCamera defaults | Fewer concepts, less coupling |
| ✅ | **BuildMaterials/BuildMeshDraws extracted** — Renderer has clean separation of extraction steps | Readability |
| ✅ | **MaterialData::FromDesc()** — encapsulated conversion, no field-by-field copy in Renderer | Maintainability |
| ✅ | **MeshFactory::TakeMeshes()** — rvalue-qualified ownership transfer | Clear ownership semantics |
| ✅ | **Scene::AddMeshes()** — generic mesh intake, not tied to procedural or imported | Extensible |

### Remaining Problems

| # | Issue | Severity |
|---|-------|----------|
| P2 | **MaterialDesc → MaterialData rebuilt every frame** — `BuildMaterials()` re-converts via `FromDesc()` each frame even when nothing changed. | Medium |
| P4 | **No diff/change tracking** — Renderer re-extracts everything every frame. No dirty flags. | Medium |
| P7 | **No material cache in Scene** — Scene only stores `MaterialDesc`, Renderer must rebuild materials every frame. | Medium |

---

## 3. Design Options

### Option A: Imperative Callback (Current)

```cpp
class Level {
    virtual void OnLoad(Scene& scene, AssetSystem& assetSystem) = 0;  // Level calls scene APIs
};
```

**How it works:** Level receives direct references to Scene and AssetSystem. Level imperatively calls `scene.LoadGltf()`, `scene.AddMeshes()`, etc.

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
    vector<MeshRequest> meshRequests;         // Imported or procedural
    // vector<LightDesc> lights;               // Future
};

class Level {
    virtual LevelDesc BuildDescription() const = 0;  // Level returns data
};

// Scene processes it:
void Scene::LoadLevel(const Level& level, AssetSystem& assets);
```

**How it works:** Level produces a pure-data `LevelDesc`. Scene consumes it, performs all loading, and owns the results. Procedural and imported meshes are both expressed as `MeshRequest`.

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

/// Procedural primitive spawn request
struct PrimitiveRequest
{
    MeshFactory::Shape shape = MeshFactory::Shape::Box;
    std::uint32_t count = 500;
    DirectX::XMFLOAT3 center  = {0.0f, 0.0f, 50.0f};
    DirectX::XMFLOAT3 extents = {100.0f, 100.0f, 100.0f};
    std::uint32_t seed = 1337;
};

/// Mesh request — unified path for imported and procedural meshes
enum class AssetSource
{
    Imported,
    Procedural,
};

struct MeshRequest
{
    AssetSource source = AssetSource::Imported;

    // Imported mesh (used when source == Imported)
    std::filesystem::path assetPath;    // Relative path (e.g., "Sponza/Sponza.gltf")
    AssetType assetType = AssetType::Mesh;

    // Procedural mesh (used when source == Procedural)
    PrimitiveRequest procedural;
};

/// Complete declarative level description
struct LevelDesc
{
    std::vector<MeshRequest> meshRequests;
    // Future: std::vector<LightDesc> lights;
};
```

**Note:** No `CameraSetup` — camera is managed by `GameCamera` independently. Levels don't dictate camera placement.

### 5.2 Refactored Level

```cpp
class Level {
    virtual std::string_view GetName() const = 0;
    virtual std::string_view GetDescription() const = 0;
    virtual LevelDesc BuildDescription() const = 0;  // NEW: returns data only
};
```

`OnLoad` / `OnUnload` are removed. Level becomes a pure data factory.

### 5.3 Refactored Scene

Scene already has the right container shape (`m_meshes`, `AddMeshes()`). `LoadLevel` changes from calling `level.OnLoad()` to consuming the descriptor:

```cpp
void Scene::LoadLevel(const Level& level, AssetSystem& assetSystem)
{
    Clear();

    LevelDesc desc = level.BuildDescription();

    // Mesh requests (imported + procedural)
    for (const auto& req : desc.meshRequests)
    {
        if (req.source == AssetSource::Imported)
        {
            auto resolved = assetSystem.ResolvePath(req.assetPath, req.assetType);
            if (resolved)
                LoadGltf(*resolved);
            else
                LOG_WARNING("Scene: Asset not found — " + req.assetPath.string());
        }
        else
        {
            MeshFactory factory;
            factory.AppendShapes(
                req.procedural.shape, req.procedural.count,
                req.procedural.center, req.procedural.extents,
                req.procedural.seed);
            AddMeshes(std::move(factory).TakeMeshes());
        }
    }

    m_currentLevelName = std::string(level.GetName());
}
```

### 5.4 Example: SponzaLevel (Before → After)

**Before (Imperative — current):**
```cpp
void SponzaLevel::OnLoad(Scene& scene, AssetSystem& assetSystem)
{
    auto path = assetSystem.ResolvePath("Sponza/Sponza.gltf", AssetType::Mesh);
    if (path) scene.LoadGltf(*path);
}
```

**After (Declarative):**
```cpp
LevelDesc SponzaLevel::BuildDescription() const
{
    LevelDesc desc;
    MeshRequest req;
    req.source = AssetSource::Imported;
    req.assetPath = "Sponza/Sponza.gltf";
    req.assetType = AssetType::Mesh;
    desc.meshRequests.push_back(req);
    return desc;
}
```

The level no longer includes Scene.h. It no longer knows how loading works. It just says *what* it wants.

### 5.5 Example: BasicShapesLevel (Before → After)

**Before (Imperative — current):**
```cpp
void BasicShapesLevel::OnLoad(Scene& scene, AssetSystem& /*assetSystem*/)
{
    MeshFactory factory;
    factory.AppendShapes(MeshFactory::Shape::Box, 500, ...);
    std::vector<std::unique_ptr<Mesh>> meshes = std::move(factory).TakeMeshes();
    scene.AddMeshes(std::move(meshes));
}
```

**After (Declarative):**
```cpp
LevelDesc BasicShapesLevel::BuildDescription() const
{
    LevelDesc desc;
    MeshRequest req;
    req.source = AssetSource::Procedural;
    req.procedural = PrimitiveRequest{
        MeshFactory::Shape::Box, 500,
        {0.0f, 0.0f, 50.0f}, {100.0f, 100.0f, 100.0f}, 1337};
    desc.meshRequests.push_back(req);
    return desc;
}
```

BasicShapesLevel no longer creates MeshFactory or calls Scene. MeshFactory creation moves to Scene::LoadLevel.

### 5.6 Benefits Delivered

| Problem | Resolution |
|---------|------------|
| P1: Level performs loading | Level returns data; Scene loads |
| P3: No declarative description | LevelDesc is serializable pure data |
| P6: Level coupling | Deleted — Level doesn't reference Scene at all |
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

Current: rebuilt every frame in `Renderer::BuildMaterials()` via `MaterialData::FromDesc()`.

Proposed optimization (Phase 3): cache `vector<MaterialData>` alongside `vector<MaterialDesc>` in Scene. Rebuild only on `LoadLevel`/`LoadGltf`. Renderer reads `Scene::GetMaterials()` directly — zero per-frame conversion.

```cpp
// Scene.h (future):
bool LoadGltf(const std::filesystem::path& filePath);
const std::vector<MaterialData>& GetCachedMaterials() const noexcept;

// Scene.cpp (future):
void Scene::RebuildMaterialCache()
{
    m_cachedMaterials.clear();
    m_cachedMaterials.reserve(m_loadedMaterials.size());
    for (const auto& desc : m_loadedMaterials)
        m_cachedMaterials.push_back(MaterialData::FromDesc(desc));
}
```

This is a **measured optimization** — profile first, optimize second. At ~500 meshes the cost is negligible.

---

## 7. Data Ownership Summary

```
┌─────────────────────────────────────────────────────────────────┐
│                     DATA OWNERSHIP MAP                          │
├─────────────┬──────────────────────┬───────────────────────────┤
│ Layer        │ Owns                 │ Produces                  │
├─────────────┼──────────────────────┼───────────────────────────┤
│ Level        │ (nothing)            │ LevelDesc (value type)    │
│ Scene        │ GameCamera, Meshes,  │ GetMeshes(),              │
│              │ MaterialDescs        │ GetLoadedMaterials()      │
│ Renderer     │ RenderCamera,        │ SceneView (per frame)     │
│              │ GPUMeshCache,        │                           │
│              │ FrameGraph           │                           │
│ RenderPass   │ (nothing)            │ GPU commands              │
└─────────────┴──────────────────────┴───────────────────────────┘
```

**Note:** Scene no longer owns MeshFactory. MeshFactory is a local tool used during level loading (by Scene::LoadLevel when processing `MeshRequest` with `AssetSource::Procedural`). Ownership is transient, not persistent.

---

## 8. Migration Path

| Phase | Work | Status | Complexity |
|-------|------|--------|------------|
| **Phase 0** | Clean Scene (remove MeshFactory/PrimitiveConfig, unify mesh vector, add AddMeshes, remove CameraSetup) | ✅ Done | Small |
| **Phase 1** (next) | Introduce `LevelDesc`, refactor 3 built-in levels, remove `OnLoad` | 🔄 Ready | Small — ~150 lines changed |
| **Phase 2** (future) | Add `LightDesc` to `LevelDesc`, wire into SceneView | Not started | Small |
| **Phase 3** (future) | Cache materials in Scene, SceneView as Renderer member | Not started | Medium |
| **Phase 4** (future) | Entity-Component archetype in LevelDesc (Option C migration) | Not started | Large |

---

## 8.1 Phase 1 Implementation Plan — Declarative Descriptor

Detailed step-by-step plan for migrating from imperative `OnLoad` to declarative `LevelDesc`.

**Prerequisites already completed (Phase 0):**
- ✅ Scene is a pure container (no MeshFactory, no PrimitiveConfig)
- ✅ `Scene::AddMeshes()` accepts externally-created meshes
- ✅ `MeshFactory::TakeMeshes()` transfers ownership
- ✅ Unified `m_meshes` vector (no imported-vs-procedural split)
- ✅ CameraSetup removed from Level (camera uses GameCamera defaults)
- ✅ `MaterialData::FromDesc()` encapsulates material conversion
- ✅ `BuildMaterials()` / `BuildMeshDraws()` extracted in Renderer

### Step 1: Create `LevelDesc.h` and `MeshRequest`

**File:** `Engine/GameFramework/Public/Level/LevelDesc.h`

Create the descriptor structs that replace imperative level loading. Pure value types with no behavior.

```cpp
#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "GameFramework/Public/Scene/MeshFactory.h"     // MeshFactory::Shape
#include "GameFramework/Public/Assets/AssetTypes.h"     // AssetType

#include <DirectXMath.h>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

/// Procedural primitive spawn request
struct SPARKLE_ENGINE_API PrimitiveRequest
{
    MeshFactory::Shape shape = MeshFactory::Shape::Box;
    std::uint32_t count = 500;
    DirectX::XMFLOAT3 center  = {0.0f, 0.0f, 50.0f};
    DirectX::XMFLOAT3 extents = {100.0f, 100.0f, 100.0f};
    std::uint32_t seed = 1337;
};

/// Mesh request — unified path for imported and procedural meshes
enum class AssetSource
{
    Imported,
    Procedural,
};

struct SPARKLE_ENGINE_API MeshRequest
{
    AssetSource source = AssetSource::Imported;

    // Imported mesh (used when source == Imported)
    std::filesystem::path assetPath;                // Relative (e.g., "Sponza/Sponza.gltf")
    AssetType assetType = AssetType::Mesh;

    // Procedural mesh (used when source == Procedural)
    PrimitiveRequest procedural;
};

/// Complete declarative level description — everything a Scene needs to build
struct SPARKLE_ENGINE_API LevelDesc
{
    std::vector<MeshRequest> meshRequests;
    // Future: std::vector<LightDesc> lights;
};
```

**Note:** No camera field — levels don't dictate camera placement (CameraSetup was removed).

**Verification:** Header-only, no `.cpp` needed. Compiles if included from any GameFramework source.

---

### Step 2: Refactor `Level` base class — replace `OnLoad` with `BuildDescription()`

**File:** `Engine/GameFramework/Public/Level/Level.h`

Changes:
- Add `#include "LevelDesc.h"`
- Add pure virtual `BuildDescription()` returning `LevelDesc`
- Remove `OnLoad()` and `OnUnload()` virtual methods

```cpp
// BEFORE:
virtual void OnLoad(Scene& scene, AssetSystem& assetSystem) = 0;
virtual void OnUnload();

// AFTER:
#include "GameFramework/Public/Level/LevelDesc.h"
...
[[nodiscard]] virtual LevelDesc BuildDescription() const = 0;
// OnLoad / OnUnload removed entirely
```

**Verification:** Compile will fail on all Level subclasses — expected. Fixed in Step 3.

---

### Step 3: Refactor built-in levels to return `LevelDesc`

Update each of the 3 built-in levels to implement `BuildDescription()` instead of `OnLoad()`.

#### 3a: `EmptyLevel.h`

```cpp
// BEFORE:
void OnLoad(Scene& /*scene*/, AssetSystem& /*assetSystem*/) override { /* empty */ }

// AFTER:
LevelDesc BuildDescription() const override
{
    return {};  // Empty level — no mesh requests
}
```

Remove any Scene/AssetSystem includes — no longer needed.

#### 3b: `BasicShapesLevel.h`

```cpp
// BEFORE:
void OnLoad(Scene& scene, AssetSystem& /*assetSystem*/) override
{
    MeshFactory factory;
    factory.AppendShapes(MeshFactory::Shape::Box, 500, ...);
    std::vector<std::unique_ptr<Mesh>> meshes = std::move(factory).TakeMeshes();
    scene.AddMeshes(std::move(meshes));
}

// AFTER:
LevelDesc BuildDescription() const override
{
    LevelDesc desc;
    MeshRequest req;
    req.source = AssetSource::Procedural;
    req.procedural = PrimitiveRequest{
        MeshFactory::Shape::Box, 500,
        {0.0f, 0.0f, 50.0f}, {100.0f, 100.0f, 100.0f}, 1337};
    desc.meshRequests.push_back(req);
    return desc;
}
```

Remove `#include "Scene/Scene.h"` and `#include "Scene/MeshFactory.h"`.

#### 3c: `SponzaLevel.h` + `SponzaLevel.cpp`

```cpp
// BEFORE (SponzaLevel.cpp):
void SponzaLevel::OnLoad(Scene& scene, AssetSystem& assetSystem)
{
    auto path = assetSystem.ResolvePath("Sponza/Sponza.gltf", AssetType::Mesh);
    if (path) scene.LoadGltf(*path);
}

// AFTER (SponzaLevel.h, inline):
LevelDesc BuildDescription() const override
{
    LevelDesc desc;
    MeshRequest req;
    req.source = AssetSource::Imported;
    req.assetPath = "Sponza/Sponza.gltf";
    req.assetType = AssetType::Mesh;
    desc.meshRequests.push_back(req);
    return desc;
}
```

**Delete `SponzaLevel.cpp`** — no longer needed. Path resolution moves to Scene::LoadLevel.

**Verification:** All 3 levels compile with `BuildDescription()`. No level includes Scene.h.

---

### Step 4: Delete `OnUnload` and leftover `OnLoad` artifacts

**Edit `Level.cpp`:** Remove `OnUnload` default impl. If Level.cpp becomes empty (no remaining implementations), delete it too.

---

### Step 5: Refactor `Scene::LoadLevel()` to consume `LevelDesc`

**File:** `Engine/GameFramework/Public/Scene/Scene.h`

Change signature:
```cpp
// BEFORE:
void LoadLevel(Level& level, AssetSystem& assetSystem);

// AFTER:
void LoadLevel(const Level& level, AssetSystem& assetSystem);
```

**File:** `Engine/GameFramework/Private/Scene/Scene.cpp`

```cpp
void Scene::LoadLevel(const Level& level, AssetSystem& assetSystem)
{
    LOG_INFO("Scene: Loading level '" + std::string(level.GetName()) + "'");

    Clear();

    LevelDesc desc = level.BuildDescription();

    // Mesh requests (imported + procedural)
    for (const auto& req : desc.meshRequests)
    {
        if (req.source == AssetSource::Imported)
        {
            auto resolved = assetSystem.ResolvePath(req.assetPath, req.assetType);
            if (resolved)
            {
                LoadGltf(*resolved);
            }
            else
            {
                LOG_WARNING("Scene: Asset not found — " + req.assetPath.string());
            }
        }
        else
        {
            MeshFactory factory;
            factory.AppendShapes(
                req.procedural.shape, req.procedural.count,
                req.procedural.center, req.procedural.extents,
                req.procedural.seed);

            std::vector<std::unique_ptr<Mesh>> meshes = std::move(factory).TakeMeshes();
            AddMeshes(std::move(meshes));
        }
    }

    m_currentLevelName = std::string(level.GetName());
    LOG_INFO("Scene: Level '" + m_currentLevelName + "' loaded");
}
```

**Key change:** Scene drives all loading. MeshFactory is created locally and destroyed after meshes are transferred — no persistent ownership.

Remove includes:
- Add `#include "Level/LevelDesc.h"`
- Add `#include "Scene/MeshFactory.h"` (for PrimitiveRequest processing)

**Verification:** `Scene::LoadLevel` compiles. App.cpp unchanged.

---

### Step 6: Clean up and verify

Run these checks:

```
1. grep -r "OnLoad"         Engine/GameFramework/  → zero hits
2. grep -r "OnUnload"       Engine/GameFramework/  → zero hits
3. grep -r "BuildDescription" Engine/GameFramework/ → Level.h + 3 built-in levels
4. cmake --build build --config Debug              → clean build
```

---

### Step 7: Verify data flow end-to-end

Post-build verification:

```
App::Initialize()
  └── LevelRegistry::FindLevelOrDefault("Sponza")
       └── returns SponzaLevel*

Scene::LoadLevel(const SponzaLevel&, assetSystem)
  └── SponzaLevel::BuildDescription()
    └── returns LevelDesc { meshRequests: ["Sponza/Sponza.gltf"] }
  └── Scene resolves path via AssetSystem
  └── Scene calls LoadGltf() internally
  └── Scene stores meshes + materials in m_meshes / m_loadedMaterials

Renderer::BuildSceneView()
  └── BuildMaterials()  — reads GetLoadedMaterials(), converts via FromDesc()
  └── BuildMeshDraws()  — reads GetMeshes(), emits MeshDraw per mesh
  └── returns SceneView { camera, meshDraws, materials, sunLight }

ForwardOpaquePass::Execute()
  └── draws meshDraws with GPUMeshCache + material data
```

No change to Renderer or RenderPass code. The refactoring is contained entirely within GameFramework.

---

### Files Changed Summary

| Action | File | Notes |
|--------|------|-------|
| **Create** | `GameFramework/Public/Level/LevelDesc.h` | ~35 lines — MeshRequest, PrimitiveRequest, LevelDesc |
| **Edit** | `GameFramework/Public/Level/Level.h` | Remove `OnLoad`/`OnUnload`, add `BuildDescription` |
| **Edit** | `GameFramework/Private/Level/Level.cpp` | Remove `OnUnload` impl; delete file if empty |
| **Edit** | `GameFramework/Private/Level/Levels/EmptyLevel.h` | `OnLoad` → `BuildDescription` |
| **Edit** | `GameFramework/Private/Level/Levels/BasicShapesLevel.h` | `OnLoad` → `BuildDescription`, remove Scene/MeshFactory includes |
| **Edit** | `GameFramework/Private/Level/Levels/SponzaLevel.h` | `OnLoad` → `BuildDescription` (inline) |
| **Delete** | `GameFramework/Private/Level/Levels/SponzaLevel.cpp` | Entire file — level is now header-only |
| **Edit** | `GameFramework/Public/Scene/Scene.h` | `LoadLevel` takes `const Level&` |
| **Edit** | `GameFramework/Private/Scene/Scene.cpp` | Consume `LevelDesc`, use MeshFactory locally for procedural mesh requests |
| **None** | `Application/Private/App.cpp` | No changes needed |
| **None** | `Renderer/Private/Renderer.cpp` | No changes needed |

**Total:** ~150 lines changed, 2 files deleted, 1 file created. Zero Renderer changes.

---

## 9. Interview Talking Points

This design demonstrates several principles interviewers at NVIDIA/AMD/Epic value:

1. **Data-Oriented Design** — LevelDesc is a POD-like struct. No inheritance, no virtuals for the data itself. Cache-friendly, serializable.

2. **Separation of Concerns** — Level doesn't know about Scene internals. Scene doesn't know about GPU. Renderer doesn't know about files.

3. **Layered Architecture** — Strict dependency direction: Level → LevelDesc → Scene → SceneView → RenderPass. No circular dependencies.

4. **YAGNI + Migration Path** — We don't build ECS now, but the descriptor pattern is a natural stepping stone. Shows pragmatic engineering judgment.

5. **Performance Awareness** — We acknowledge full extraction is O(n) per frame but justify it with measured reasoning, not premature optimization.

6. **Real-Engine Precedent** — UE5's `FWorldPartitionActorDesc`, Frostbite's data-driven SubLevels, Unity's `SceneAsset` all follow the descriptor pattern.
