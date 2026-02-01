# Option C: Hybrid Plugin Architecture (Unity/O3DE-Style)

**Reference Engines:** Unity Engine, O3DE (Amazon Lumberyard), Bevy

---

## Overview

The Hybrid Plugin Architecture uses a minimal core with dynamically loadable "Gems" (plugins). Each gem is a self-contained feature module that can be loaded on demand, hot-reloaded during development, and swapped between projects.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                            Game Project                                  │
│                         (MyGame.Gameplay.dll)                           │
└─────────────────────────────────────────────────────────────────────────┘
                                   │
         ┌─────────────────────────┼─────────────────────────┐
         ▼                         ▼                         ▼
┌─────────────────┐    ┌─────────────────────┐    ┌─────────────────┐
│ Sparkle.Editor  │    │     Packages        │    │    Modules      │
│   (optional)    │    │ (PostProcessing,    │    │ (Rendering,     │
│                 │    │  GLTF, etc.)        │    │  Input, UI)     │
└─────────────────┘    └─────────────────────┘    └─────────────────┘
         │                         │                         │
         └─────────────────────────┼─────────────────────────┘
                                   ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                         Sparkle.Engine.dll                               │
│                    (World, Entity, Component, Assets)                    │
└─────────────────────────────────────────────────────────────────────────┘
                                   │
                   ┌───────────────┴───────────────┐
                   ▼                               ▼
┌───────────────────────────────┐   ┌───────────────────────────────┐
│       Sparkle.Platform        │   │        Sparkle.Core           │
│   (Application, Window, OS)   │   │   (Math, Memory, Jobs, Log)   │
└───────────────────────────────┘   └───────────────────────────────┘
```

---

## Directory Structure

```
SparkleEngine/
│
├── Code/
│   │
│   ├── Framework/                     # Always loaded - minimal core
│   │   │
│   │   ├── Core/                      ═════════════════════════════════╗
│   │   │   ├── Sparkle.Core.dll       ║ Sparkle.Core.dll               ║
│   │   │   ├── Interface/             ║ ─────────────────────────      ║
│   │   │   │   ├── IModule.h          ║ • Module loading system        ║
│   │   │   │   ├── ISystem.h          ║ • Reflection                   ║
│   │   │   │   └── IBus.h             ║ • Event bus                    ║
│   │   │   ├── Reflection/            ║ • Serialization                ║
│   │   │   │   ├── TypeRegistry.h     ║                                ║
│   │   │   │   └── SerializeContext.h ║ TINY footprint: <1 MB          ║
│   │   │   ├── Memory/                ╚═════════════════════════════════╝
│   │   │   │   ├── Allocator.h
│   │   │   │   └── Pool.h
│   │   │   ├── Math/
│   │   │   │   ├── Vector.h
│   │   │   │   └── Matrix.h
│   │   │   └── Jobs/
│   │   │       ├── JobSystem.h
│   │   │       └── TaskGraph.h
│   │   │
│   │   └── Platform/                  ═════════════════════════════════╗
│   │       ├── Sparkle.Platform.dll   ║ Sparkle.Platform.dll           ║
│   │       ├── Application.h          ║ ─────────────────────────      ║
│   │       ├── Window.h               ║ • Application lifecycle        ║
│   │       └── NativeWindow/          ║ • Window management            ║
│   │           ├── WindowsWindow.cpp  ║ • Input routing                ║
│   │           └── LinuxWindow.cpp    ╚═════════════════════════════════╝
│   │
│   └── Gems/                          # Loadable feature modules
│       │
│       ├── RHI/                       ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.RHI.dll                ║
│       │   ├── Code/                  ║ ─────────────────────────      ║
│       │   │   ├── Include/           ║ Abstract RHI interfaces        ║
│       │   │   │   ├── RHI.h          ║                                ║
│       │   │   │   ├── Device.h       ║ • No backend code here         ║
│       │   │   │   ├── CommandList.h  ║ • Just interfaces              ║
│       │   │   │   └── Texture.h      ║                                ║
│       │   │   └── Source/            ║ Depends: Core                  ║
│       │   └── CMakeLists.txt         ╚═════════════════════════════════╝
│       │
│       ├── RHI.D3D12/                 ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.RHI.D3D12.dll          ║
│       │   ├── Code/                  ║ ─────────────────────────      ║
│       │   │   └── Source/            ║ D3D12 implementation           ║
│       │   │       ├── D3D12Device.cpp║                                ║
│       │   │       ├── D3D12CmdList.cpp║ Implements: RHI interfaces    ║
│       │   │       └── D3D12Memory.cpp║                                ║
│       │   └── CMakeLists.txt         ║ Depends: Core, RHI             ║
│       │                              ╚═════════════════════════════════╝
│       │
│       ├── RHI.Vulkan/                ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.RHI.Vulkan.dll         ║
│       │   └── Code/                  ║ ─────────────────────────      ║
│       │       └── Source/            ║ Vulkan implementation          ║
│       │           └── ...            ║ Implements: RHI interfaces     ║
│       │                              ╚═════════════════════════════════╝
│       │
│       ├── Rendering/                 ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.Rendering.dll          ║
│       │   ├── Code/                  ║ ─────────────────────────      ║
│       │   │   ├── Include/           ║ High-level rendering           ║
│       │   │   │   ├── SceneRenderer.h║                                ║
│       │   │   │   ├── RenderPipeline.h║ • Render passes               ║
│       │   │   │   └── Material.h     ║ • Materials                    ║
│       │   │   └── Source/            ║ • Lighting                     ║
│       │   │       ├── Passes/        ║                                ║
│       │   │       │   ├── DepthPass.cpp║ Depends: Core, RHI           ║
│       │   │       │   └── GBufferPass.cpp                             ║
│       │   │       └── Shaders/       ╚═════════════════════════════════╝
│       │   └── CMakeLists.txt
│       │
│       ├── Entity/                    ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.Entity.dll             ║
│       │   ├── Code/                  ║ ─────────────────────────      ║
│       │   │   ├── Include/           ║ Entity-Component System        ║
│       │   │   │   ├── EntityId.h     ║                                ║
│       │   │   │   ├── Component.h    ║ • Entity management            ║
│       │   │   │   ├── System.h       ║ • Component registry           ║
│       │   │   │   └── World.h        ║ • System scheduling            ║
│       │   │   └── Source/            ║                                ║
│       │   └── CMakeLists.txt         ║ Depends: Core                  ║
│       │                              ╚═════════════════════════════════╝
│       │
│       ├── Physics/                   ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.Physics.dll            ║
│       │   └── Code/                  ║ ─────────────────────────      ║
│       │       ├── Include/           ║ Physics interfaces             ║
│       │       │   ├── PhysicsWorld.h ║                                ║
│       │       │   └── RigidBody.h    ║ Abstract collision/dynamics    ║
│       │       └── Source/            ║                                ║
│       │                              ║ Depends: Core, Entity          ║
│       │                              ╚═════════════════════════════════╝
│       │
│       ├── Physics.Jolt/              ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.Physics.Jolt.dll       ║
│       │   └── Code/                  ║ ─────────────────────────      ║
│       │       └── Source/            ║ Jolt Physics backend           ║
│       │           └── JoltWorld.cpp  ║                                ║
│       │                              ║ Implements: Physics interfaces ║
│       │                              ╚═════════════════════════════════╝
│       │
│       ├── Physics.PhysX/             # PhysX backend (alternative)
│       │   └── ...
│       │
│       ├── Animation/                 ═════════════════════════════════╗
│       │   ├── Gem.json               ║ Sparkle.Animation.dll          ║
│       │   └── Code/                  ║ ─────────────────────────      ║
│       │       ├── Include/           ║ Animation system               ║
│       │       │   ├── Skeleton.h     ║                                ║
│       │       │   ├── AnimGraph.h    ║ • Skeletal animation           ║
│       │       │   └── IK.h           ║ • Blend trees                  ║
│       │       └── Source/            ║ • IK solvers                   ║
│       │                              ║                                ║
│       │                              ║ Depends: Core, Entity,Rendering║
│       │                              ╚═════════════════════════════════╝
│       │
│       ├── Audio/                     # Audio interfaces
│       │   └── ...
│       │
│       ├── Audio.FMOD/                # FMOD backend
│       │   └── ...
│       │
│       ├── Audio.Wwise/               # Wwise backend
│       │   └── ...
│       │
│       ├── AI/                        # AI interfaces
│       │   └── ...
│       │
│       ├── AI.Recast/                 # Recast NavMesh backend
│       │   └── ...
│       │
│       ├── Networking/                # Networking gem
│       │   └── ...
│       │
│       ├── UI/                        # UI gem
│       │   └── ...
│       │
│       ├── Scripting/                 # Scripting interfaces
│       │   └── ...
│       │
│       ├── Scripting.Lua/             # Lua backend
│       │   └── ...
│       │
│       └── EditorFramework/           # Editor gem
│           └── ...
│
├── Tools/
│   ├── Editor/
│   │   ├── EditorApplication.cpp
│   │   └── ...
│   │
│   ├── AssetProcessor/
│   │   └── ...
│   │
│   └── ShaderCompiler/
│       └── ...
│
├── Projects/
│   │
│   ├── MyGame/
│   │   │
│   │   ├── project.json               ═════════════════════════════════╗
│   │   │   {                          ║ Project Manifest               ║
│   │   │     "name": "MyGame",        ║ ─────────────────────────      ║
│   │   │     "gems": [                ║ Lists which gems to load       ║
│   │   │       "Sparkle.RHI.D3D12",   ║                                ║
│   │   │       "Sparkle.Rendering",   ║ Different projects can use     ║
│   │   │       "Sparkle.Entity",      ║ different backends!            ║
│   │   │       "Sparkle.Physics.Jolt",║                                ║
│   │   │       "Sparkle.Animation",   ║ MyGame → Physics.Jolt          ║
│   │   │       "Sparkle.Audio.FMOD",  ║ OtherGame → Physics.PhysX      ║
│   │   │       "Sparkle.AI.Recast",   ║                                ║
│   │   │       "MyGame.Gameplay"      ╚═════════════════════════════════╝
│   │   │     ]
│   │   │   }
│   │   │
│   │   ├── Gems/
│   │   │   └── Gameplay/              # Game-specific gem
│   │   │       ├── Gem.json
│   │   │       └── Code/
│   │   │           ├── Include/
│   │   │           │   └── Player.h
│   │   │           └── Source/
│   │   │               └── Player.cpp
│   │   │
│   │   └── Assets/
│   │       ├── Levels/
│   │       ├── Characters/
│   │       └── ...
│   │
│   └── Sponza/
│       └── ...
│
├── Engine/
│   ├── Config/
│   │   └── GemRegistry.json           # All available gems
│   │
│   └── Launcher/
│       ├── GameLauncher/              # SparkleGame.exe
│       │   └── main.cpp
│       └── EditorLauncher/            # SparkleEditor.exe
│           └── main.cpp
│
└── ThirdParty/
    └── ...
```

---

## Key Concepts

### 1. Gem Interface

Every gem implements a standard interface:

```cpp
// Framework/Core/Interface/IModule.h
class IGem
{
public:
    virtual ~IGem() = default;
    
    // Lifecycle
    virtual void OnActivate(GemContext& context) = 0;
    virtual void OnDeactivate() = 0;
    
    // Registration
    virtual void RegisterTypes(TypeRegistry& registry) = 0;
    virtual void RegisterSystems(SystemRegistry& registry) = 0;
};

// Gem.json structure
{
    "name": "Sparkle.Physics.Jolt",
    "version": "1.0.0",
    "dependencies": [
        { "name": "Sparkle.Core", "version": ">=1.0" },
        { "name": "Sparkle.Entity", "version": ">=1.0" }
    ],
    "provides": [
        "PhysicsWorld",
        "RigidBodyComponent"
    ],
    "implements": "Sparkle.Physics"
}
```

### 2. Gem Manager

Loads and manages gems at runtime:

```cpp
class GemManager
{
public:
    void LoadGemsFromProject(const std::string& projectPath)
    {
        auto project = LoadJson(projectPath + "/project.json");
        
        for (const auto& gemName : project["gems"])
        {
            LoadGem(gemName);
        }
    }
    
    void LoadGem(const std::string& name)
    {
        // 1. Read Gem.json for dependencies
        auto gemJson = FindGem(name);
        
        // 2. Load dependencies first (topological sort)
        for (const auto& dep : gemJson["dependencies"])
        {
            if (!IsLoaded(dep["name"]))
                LoadGem(dep["name"]);
        }
        
        // 3. Load DLL
        auto handle = LoadLibrary((name + ".dll").c_str());
        
        // 4. Get entry point
        auto createFunc = GetProcAddress(handle, "CreateGem");
        auto gem = createFunc();
        
        // 5. Activate
        gem->OnActivate(m_context);
        gem->RegisterTypes(m_typeRegistry);
        gem->RegisterSystems(m_systemRegistry);
        
        m_loadedGems[name] = gem;
    }
    
    void HotReloadGem(const std::string& name)
    {
        // 1. Deactivate old gem
        auto oldGem = m_loadedGems[name];
        oldGem->OnDeactivate();
        
        // 2. Unload DLL
        FreeLibrary(m_handles[name]);
        
        // 3. Load new DLL
        LoadGem(name);
        
        // 4. Restore state (complex!)
    }
};
```

### 3. Dependency Resolution

```
Request: Load "MyGame.Gameplay"

                    ┌─────────────────┐
                    │ MyGame.Gameplay │
                    └────────┬────────┘
                             │
          ┌──────────────────┼──────────────────┐
          │                  │                  │
          ▼                  ▼                  ▼
   ┌────────────┐    ┌─────────────┐    ┌─────────────┐
   │  Animation │    │   Physics   │    │    Audio    │
   │            │    │   (Jolt)    │    │   (FMOD)    │
   └──────┬─────┘    └──────┬──────┘    └──────┬──────┘
          │                 │                  │
          └────────┬────────┴────────┬─────────┘
                   │                 │
                   ▼                 ▼
            ┌────────────┐    ┌────────────┐
            │  Rendering │    │   Entity   │
            └──────┬─────┘    └──────┬─────┘
                   │                 │
                   └────────┬────────┘
                            │
                            ▼
                     ┌─────────────┐
                     │ RHI.D3D12   │
                     └──────┬──────┘
                            │
                            ▼
                     ┌─────────────┐
                     │     RHI     │
                     └──────┬──────┘
                            │
                            ▼
                     ┌─────────────┐
                     │    Core     │
                     └─────────────┘

LOAD ORDER (topological sort):
1. Core → 2. RHI → 3. RHI.D3D12 → 4. Entity → 5. Rendering →
6. Animation → 7. Physics.Jolt → 8. Audio.FMOD → 9. MyGame.Gameplay
```

### 4. Service Locator Pattern

Gems register services that other gems can use:

```cpp
// Physics.Jolt gem registration
void PhysicsJoltGem::OnActivate(GemContext& ctx)
{
    // Register as the physics provider
    ctx.RegisterService<IPhysicsWorld>(
        std::make_unique<JoltPhysicsWorld>()
    );
}

// Game code uses interface
void Player::OnUpdate(float delta)
{
    auto* physics = GetService<IPhysicsWorld>();
    physics->Raycast(position, direction, hit);
}
```

---

## Build Impact Analysis

```
┌─────────────────────┬───────────────────────────────────────────────────────┐
│ File Changed        │ Impact                                                │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Core/Math/Vector.h  │ Recompile: ████████████████████████ All gems          │
│                     │ Relink:    Each gem separately                        │
│                     │ Time: ~60 seconds (parallel)                          │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ RHI/Device.h        │ Recompile: ████████ RHI backends + Rendering          │
│                     │ Relink:    4 gem DLLs                                 │
│                     │ Time: ~20 seconds                                     │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Physics/RigidBody.h │ Recompile: ██ Physics gem only                        │
│                     │ Relink:    1 gem DLL                                  │
│                     │ Time: ~8 seconds                                      │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Game/Player.cpp     │ Recompile: █ That file                                │
│                     │ Relink:    1 gem DLL (MyGame.Gameplay)                │
│                     │ Time: ~5 seconds                                      │
│                     │ HOT RELOAD: ~2 seconds (no restart!)                  │
└─────────────────────┴───────────────────────────────────────────────────────┘
```

---

## Advantages

### 🔥 Hot Reload

```
Editor running
     │
     ▼
Modify Player.cpp
     │
     ▼
Rebuild MyGame.Gameplay.dll (5 sec)
     │
     ▼
Gem hot-reload
     │
     ▼
Continue testing (no restart!)

Total iteration: ~10 seconds
```

### 🎛️ Backend Swapping

Switch backends per-project:

```json
// PC Project
{
    "gems": [
        "Sparkle.RHI.D3D12",
        "Sparkle.Physics.Jolt",
        "Sparkle.Audio.FMOD"
    ]
}

// Console Project
{
    "gems": [
        "Sparkle.RHI.Vulkan",
        "Sparkle.Physics.PhysX",
        "Sparkle.Audio.Wwise"
    ]
}
```

Same game code, different backends!

### 📦 Minimal Footprint

Only load what you need:

```
Minimal game:
• Core         (1 MB)
• RHI          (2 MB)
• Rendering    (8 MB)
• Entity       (1 MB)
─────────────────────
Total:        12 MB

vs Full engine: 100+ MB
```

### 🔌 Extensibility

Third parties can create gems:

```
Community/Marketplace:
• Sparkle.VR.OpenXR
• Sparkle.Terrain
• Sparkle.Vegetation
• Sparkle.Weather
• Sparkle.DialogueSystem
• Sparkle.ProceduralGen

Just add to project.json!
```

---

## Disadvantages

### 🏗️ Infrastructure Overhead

Need to build significant infrastructure:

```
Required systems:
• GemManager
• Dependency resolver
• Hot-reload system
• Service locator
• Gem registry
• Gem.json parser
• Type registry across DLLs

Estimate: 2-3 months before shipping features
```

### 🔗 Interface Explosion

Every system needs abstract interface:

```cpp
IPhysicsWorld
  └─► JoltPhysicsWorld
  └─► PhysXPhysicsWorld

IRenderPipeline
  └─► ForwardPipeline
  └─► DeferredPipeline

IAudioSystem
  └─► FMODAudioSystem
  └─► WwiseAudioSystem

// Virtual call overhead on every operation
```

### 🐛 Debugging Complexity

```
• 20+ DLLs to debug
• Symbol servers needed
• Hot-reload state bugs
• Registration order matters

"Why isn't my component showing up?"
    │
    └─► Registration failed silently in DLL load
```

### 📋 Version Management

Diamond dependency problem:

```
       ┌─────┐
       │ Game│
       └──┬──┘
     ┌────┴────┐
     ▼         ▼
  ┌─────┐  ┌─────┐
  │ A   │  │ B   │
  │v1.2 │  │v2.0 │
  └──┬──┘  └──┬──┘
     └────┬────┘
          ▼
    ┌───────────┐
    │ Entity ?? │  Which version?
    │ v1.0? v2.0│
    └───────────┘
```

---

## When to Use This Architecture

✅ **Good for:**
- Large teams (50+ developers)
- Long-lived engine projects
- Need hot-reload for fast iteration
- Multiple games sharing engine
- Want marketplace/ecosystem

❌ **Avoid if:**
- Small team / solo developer
- Simple project
- Need maximum runtime performance
- Don't have months for infrastructure

---

## Migration from Current State

**Timeline: 2-3 months**

```
Month 1:
├── Design gem interface
├── Build GemManager
├── Build dependency resolver
└── Create Core gem

Month 2:
├── Extract RHI gem
├── Extract Entity gem
├── Extract Rendering gem
└── Test gem loading

Month 3:
├── Add hot-reload support
├── Create remaining gems
├── Build project system
└── Documentation
```

---

## Example: Project Configuration

```json
// Projects/MyGame/project.json
{
    "name": "MyGame",
    "version": "1.0.0",
    
    "gems": [
        // Core (always needed)
        "Sparkle.Core",
        "Sparkle.Platform",
        
        // Graphics
        "Sparkle.RHI",
        "Sparkle.RHI.D3D12",
        "Sparkle.Rendering",
        
        // Gameplay
        "Sparkle.Entity",
        "Sparkle.Physics",
        "Sparkle.Physics.Jolt",
        "Sparkle.Animation",
        
        // Audio
        "Sparkle.Audio",
        "Sparkle.Audio.FMOD",
        
        // AI
        "Sparkle.AI",
        "Sparkle.AI.Recast",
        
        // Game-specific
        "MyGame.Gameplay",
        "MyGame.UI"
    ],
    
    "settings": {
        "rendering": {
            "pipeline": "deferred",
            "shadows": "cascade"
        },
        "physics": {
            "gravity": [0, -9.81, 0],
            "substeps": 2
        }
    }
}
```
