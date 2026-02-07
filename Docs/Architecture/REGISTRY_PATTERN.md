<!-- ========================================================================= -->
<!-- Registry Pattern — Design Rationale for LevelRegistry                    -->
<!-- Sparkle Engine — Architecture Decision Record                            -->
<!-- ========================================================================= -->

# Registry Pattern: LevelRegistry Design Rationale

**Author:** Engine Architecture  
**Status:** Accepted  
**Date:** 2026-02-07  

---

## 1. What Is the Registry Pattern?

A **Registry** is a well-known object that other objects use to find common objects and services. It acts as a centralized lookup table where producers **register** entries and consumers **query** them by key.

```
Producer ──register──► Registry ◄──find── Consumer
```

In our engine, `LevelRegistry` is the registry:
- **Producers:** Engine built-in levels + application-defined levels call `Register()`
- **Consumers:** `App::Initialize()` calls `FindLevel()` to select a startup level
- **Key:** Level name (`std::string_view`) or typed enum (`BuiltinLevel`)

The pattern is catalogued by Martin Fowler (*Patterns of Enterprise Application Architecture*) and is pervasive in game engines.

---

## 2. Why LevelRegistry Exists

### Problem

Without a registry, level selection requires one of:

1. **Hardcoded switch** — App knows every level type at compile time
2. **Direct construction** — App constructs level objects inline
3. **Factory function per level** — scattered creation logic

All three create tight coupling between the application layer and specific level implementations.

### Concrete Example

**Without registry (hardcoded):**
```cpp
void App::Initialize()
{
    // App must #include every level type
    // Adding a new level = editing App.cpp
    if (m_levelName == "Sponza")
        level = std::make_unique<SponzaLevel>();
    else if (m_levelName == "BasicShapes")
        level = std::make_unique<BasicShapesLevel>();
    else if (m_levelName == "Empty")
        level = std::make_unique<EmptyLevel>();
    // ... grows linearly with every new level
}
```

**With registry:**
```cpp
void App::Initialize()
{
    // App only knows about Level* — zero concrete level includes
    Level* level = LevelRegistry::Get().FindLevel(m_startupLevel);
    if (level) m_scene->LoadLevel(*level, *m_assetSystem);
}
```

### Benefits Delivered by LevelRegistry

| Benefit | How |
|---------|-----|
| **Open/Closed Principle** | New levels added via `Register()` — no existing code modified |
| **Decoupling** | App never includes concrete level headers |
| **Discoverability** | `GetAllLevels()` enables UI level selectors, debug menus |
| **Engine/App separation** | Engine registers built-ins in constructor; apps register custom levels before `Run()` |
| **Single point of truth** | One place to answer "what levels exist?" |
| **Runtime extensibility** | Plugins or DLLs could register levels without recompiling the engine |

---

## 3. Registry Pattern — General Analysis

### 3.1 Pros

| Advantage | Explanation |
|-----------|-------------|
| **Loose coupling** | Producers and consumers only depend on the registry interface, not each other |
| **Central discoverability** | Any system can enumerate all registered entries |
| **Late binding** | Entries can be registered at runtime (plugin architectures, mod support) |
| **Testability** | Tests can register mock entries without touching production code |
| **Extensibility** | Adding new entries requires zero changes to existing consumers |

### 3.2 Cons

| Disadvantage | Explanation | Mitigation |
|--------------|-------------|------------|
| **Global state** | Singletons are effectively global variables | Scope lifetime carefully; document ownership |
| **Hidden dependencies** | Code depends on registry implicitly rather than via constructor | Keep registry access at initialization boundaries, not deep in call stacks |
| **Initialization order** | Singleton must exist before first `Register()` call | Meyers singleton (function-local static) guarantees construction on first use |
| **Name collisions** | String keys can conflict | Reject duplicates with warnings (our approach) |
| **Thread safety** | Concurrent registration requires synchronization | Document single-threaded init; add mutex if needed later |
| **Debugging opacity** | Hard to trace "who registered what" | Log every registration with source info |

### 3.3 When to Use

Use a registry when:
- Multiple independent modules produce entries of the same type
- Consumers need to discover entries without knowing producers
- The set of entries changes between configurations (engine vs. app, debug vs. shipping)
- You want plugin/DLL extensibility without recompilation

Avoid when:
- Only 1–2 entries exist and will never grow
- Entries have complex interdependencies (use a builder or graph instead)
- The lookup is performance-critical per-frame (registries are for initialization-time lookup)

---

## 4. Alternatives Considered

### 4.1 Factory Function Map

```cpp
using LevelFactory = std::function<std::unique_ptr<Level>()>;
std::unordered_map<std::string, LevelFactory> g_levelFactories;

// Registration:
g_levelFactories["Sponza"] = [] { return std::make_unique<SponzaLevel>(); };

// Lookup:
auto level = g_levelFactories["Sponza"]();
```

| Pros | Cons |
|------|------|
| No singleton class needed | Returns new instance each time (ownership unclear) |
| Simple STL-only implementation | No metadata (description, camera defaults) without calling factory |
| | Can't enumerate without creating all instances |
| | Global map is still global state |

**Verdict:** Lighter weight but loses discoverability and metadata access. Equivalent to registry minus the type safety.

### 4.2 Service Locator

```cpp
class ServiceLocator {
    static LevelService* GetLevelService();
};

class LevelService {
    virtual Level* FindLevel(string_view name) = 0;
    virtual void Register(unique_ptr<Level>) = 0;
};
```

| Pros | Cons |
|------|------|
| Abstracts registry behind interface | Extra indirection with no benefit at our scale |
| Swappable implementations (test vs. prod) | More complex — interface + implementation + locator |
| Follows Dependency Inversion | Over-engineered for a single registry |

**Verdict:** Appropriate when multiple registry implementations exist (e.g., networked asset registry vs. local). Overkill for levels.

**Used by:** Unity's `Resources.Load` (service locator over asset database)

### 4.3 Dependency Injection (No Registry)

```cpp
App::App(std::vector<std::unique_ptr<Level>> levels, string startupName)
    : m_levels(std::move(levels)), ...
```

| Pros | Cons |
|------|------|
| No global state at all | Caller must construct all levels and pass them in |
| Explicit dependencies | Couples `main()` to every level type |
| Easy to test | No discoverability — no "list all levels" without the full vector |
| | Doesn't support plugin registration |

**Verdict:** Purest from a DI perspective, but impractical when levels come from both engine and application modules compiled separately.

**Used by:** Small test frameworks, research engines

### 4.4 Static Auto-Registration (CRTP / Macro)

```cpp
// Each level self-registers via a static initializer
#define REGISTER_LEVEL(Type) \
    static bool _reg_##Type = (LevelRegistry::Get().Register(std::make_unique<Type>()), true);

// In SponzaLevel.cpp:
REGISTER_LEVEL(SponzaLevel)
```

| Pros | Cons |
|------|------|
| Zero manual registration code | Relies on static initialization order (fragile) |
| Adding a level = just adding the macro | Linker may strip unused translation units |
| Common in plugin architectures | Hard to debug; implicit; macro-heavy |
| | No control over registration order |

**Verdict:** Powerful for large-scale plugin systems (DOOM, idTech). Too magical for a learning engine where explicitness aids understanding.

**Used by:** idTech (entity type registration), Unreal (UCLASS macro + reflection)

---

## 5. Industry Precedent

| Engine | Pattern | Mechanism |
|--------|---------|-----------|
| **Unreal Engine 5** | Auto-registration via `UCLASS()` macro | Unreal Header Tool generates static registrants at build time. `UGameInstance` selects maps by asset path. Effectively a registry backed by reflection. |
| **Unity** | SceneManager + Build Settings | Scenes registered in project build settings (editor GUI). `SceneManager.LoadScene()` by index or name. Registry is the build settings list. |
| **Frostbite** | Level/SubLevel asset database | Levels are assets in a database. Runtime queries the asset DB (a registry). Editor populates it. |
| **DOOM Eternal** | Static auto-registration | Entities and systems self-register via macros in translation units. idTech linker ensures nothing is stripped. |
| **Godot** | ClassDB | All node types registered in central `ClassDB`. `ClassDB::register_class<T>()`. Pure registry pattern. |

Every major engine uses some form of registry for type/level/asset discovery. The variation is in *how* entries get registered (manual, macro, reflection, editor GUI).

---

## 6. Our Design Choices

| Decision | Rationale |
|----------|-----------|
| **Meyers singleton** | Guarantees construction on first use; no static init order issues |
| **Explicit `Register()` calls** | Clear, debuggable, no macros. Engine registers in constructor; apps register in `main()` before `Run()` |
| **Reject duplicates with warning** | Fail-safe; doesn't crash, but alerts developer |
| **`BuiltinLevel` enum** | Type-safe selection for known engine levels; string fallback for app-defined levels |
| **Log every registration** | Full traceability in log output |
| **No mutex** | Single-threaded init; document assumption. Add mutex when threading arrives. |

---

## 7. Extension Points (Future)

| Feature | How Registry Enables It |
|---------|------------------------|
| **Editor level picker** | `GetAllLevels()` populates a dropdown with name + description |
| **Command-line level override** | `--level Sponza` → `FindLevel("Sponza")`, no code changes |
| **Plugin/DLL levels** | DLL entry point calls `LevelRegistry::Get().Register(...)` |
| **Level hot-reload** | Unregister + re-register without restarting engine |
| **Mod support** | Mod loader registers custom levels at startup |
| **Networked level sync** | Server sends level name; client looks up in registry |
