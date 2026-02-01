# Option A: Layered DLL Architecture (Unreal-Style)

**Reference Engines:** Unreal Engine, CryEngine

---

## Overview

The Layered DLL architecture organizes the engine into a strict hierarchy of dynamic libraries, where each layer only depends on layers below it. This creates clear boundaries and enables fast incremental builds.

```
┌─────────────────────────────────────────────────────────────┐
│                         Game Project                        │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                     SparkleEditor.dll                       │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                     SparkleEngine.dll                       │
│        (GameFramework, Camera, Scene, Assets)               │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│SparklePhysics │    │SparkleAudio   │    │ SparkleAI     │
│    .dll       │    │    .dll       │    │    .dll       │
└───────────────┘    └───────────────┘    └───────────────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    SparkleRenderer.dll                      │
│           (SceneRenderer, Materials, Passes)                │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                       SparkleRHI.dll                        │
│              (D3D12 Backend, Vulkan Backend)                │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    SparklePlatform.dll                      │
│              (Window, Input, FileSystem)                    │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      SparkleCore.dll                        │
│            (Math, Memory, Containers, Logging)              │
└─────────────────────────────────────────────────────────────┘
```

---

## Directory Structure

```
SparkleEngine/
│
├── Engine/
│   │
│   ├── Core/                          ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleCore.dll             ║
│   │   │   ├── CoreMinimal.h          ║ ────────────────────────    ║
│   │   │   ├── CoreTypes.h            ║ • Math (Vector, Matrix)     ║
│   │   │   ├── Math/                  ║ • Memory (Allocators)       ║
│   │   │   │   ├── Vector.h           ║ • Containers (Array, Map)   ║
│   │   │   │   ├── Matrix.h           ║ • Delegates/Events          ║
│   │   │   │   └── Frustum.h          ║ • Job System                ║
│   │   │   ├── Memory/                ║ • Logging                   ║
│   │   │   │   ├── Allocators.h       ║ • Reflection                ║
│   │   │   │   └── SmartPointers.h    ║                             ║
│   │   │   ├── Containers/            ║ Dependencies: None          ║
│   │   │   ├── Delegates/             ║ Rebuild: Rare               ║
│   │   │   ├── Async/                 ╚══════════════════════════════╝
│   │   │   │   ├── TaskGraph.h
│   │   │   │   └── ParallelFor.h
│   │   │   ├── Serialization/
│   │   │   └── Reflection/
│   │   ├── Private/
│   │   └── CMakeLists.txt
│   │
│   ├── Platform/                      ══════════════════════════════╗
│   │   ├── Public/                    ║ SparklePlatform.dll         ║
│   │   │   ├── PlatformMinimal.h      ║ ────────────────────────    ║
│   │   │   ├── Application/           ║ • Application lifecycle     ║
│   │   │   │   └── Application.h      ║ • Window management         ║
│   │   │   ├── Window/                ║ • Input system              ║
│   │   │   │   └── Window.h           ║ • File I/O                  ║
│   │   │   ├── Input/                 ║ • Threading                 ║
│   │   │   │   ├── InputTypes.h       ║                             ║
│   │   │   │   └── InputSystem.h      ║ Dependencies: Core          ║
│   │   │   └── FileSystem/            ║ Rebuild: OS changes         ║
│   │   ├── Private/                   ╚══════════════════════════════╝
│   │   │   ├── Windows/
│   │   │   │   ├── WindowsWindow.cpp
│   │   │   │   └── WindowsInput.cpp
│   │   │   └── Linux/
│   │   └── CMakeLists.txt
│   │
│   ├── RHI/                           ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleRHI.dll              ║
│   │   │   ├── RHI.h                  ║ ────────────────────────    ║
│   │   │   ├── RHIDefinitions.h       ║ • Abstract GPU interface    ║
│   │   │   ├── RHIDevice.h            ║ • Resource types            ║
│   │   │   ├── RHICommandList.h       ║ • Command recording         ║
│   │   │   ├── RHIResources.h         ║ • Synchronization           ║
│   │   │   │   ├── RHIBuffer.h        ║ • Shader compilation        ║
│   │   │   │   ├── RHITexture.h       ║                             ║
│   │   │   │   └── RHIPipeline.h      ║ Dependencies: Core,Platform ║
│   │   │   └── RHIShader.h            ║ Rebuild: API changes        ║
│   │   ├── Private/                   ╚══════════════════════════════╝
│   │   │   ├── D3D12/
│   │   │   │   ├── D3D12Device.cpp
│   │   │   │   ├── D3D12CommandList.cpp
│   │   │   │   ├── D3D12Resources.cpp
│   │   │   │   ├── D3D12Pipeline.cpp
│   │   │   │   ├── D3D12Descriptors.cpp
│   │   │   │   └── D3D12Memory.cpp
│   │   │   └── Vulkan/
│   │   │       └── ...
│   │   └── CMakeLists.txt
│   │
│   ├── Renderer/                      ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleRenderer.dll         ║
│   │   │   ├── Renderer.h             ║ ────────────────────────    ║
│   │   │   ├── SceneRenderer.h        ║ • Scene rendering           ║
│   │   │   ├── Camera/                ║ • Render passes             ║
│   │   │   │   └── RenderCamera.h     ║ • Materials & Shaders       ║
│   │   │   ├── Materials/             ║ • Lighting                  ║
│   │   │   │   ├── Material.h         ║ • Post-processing           ║
│   │   │   │   └── MaterialInstance.h ║                             ║
│   │   │   ├── Lighting/              ║ Dependencies: Core, RHI     ║
│   │   │   │   ├── LightTypes.h       ║ Rebuild: Frequent           ║
│   │   │   │   └── LightManager.h     ╚══════════════════════════════╝
│   │   │   └── PostProcess/
│   │   ├── Private/
│   │   │   ├── Passes/
│   │   │   │   ├── DepthPrepass.cpp
│   │   │   │   ├── GBufferPass.cpp
│   │   │   │   ├── LightingPass.cpp
│   │   │   │   ├── ShadowPass.cpp
│   │   │   │   └── PostProcessPass.cpp
│   │   │   └── Shaders/
│   │   └── CMakeLists.txt
│   │
│   ├── Engine/                        ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleEngine.dll           ║
│   │   │   ├── Engine.h               ║ ────────────────────────    ║
│   │   │   ├── World/                 ║ • World/Level management    ║
│   │   │   │   ├── World.h            ║ • Entity-Component system   ║
│   │   │   │   └── Level.h            ║ • Game object lifecycle     ║
│   │   │   ├── GameFramework/         ║ • Cameras                   ║
│   │   │   │   ├── Entity.h           ║ • Timers                    ║
│   │   │   │   ├── Component.h        ║                             ║
│   │   │   │   ├── Actor.h            ║ Dependencies: Core,Platform ║
│   │   │   │   └── GameMode.h         ║   Renderer                  ║
│   │   │   ├── Components/            ║ Rebuild: Frequent           ║
│   │   │   │   ├── TransformComp.h    ╚══════════════════════════════╝
│   │   │   │   ├── MeshComp.h
│   │   │   │   ├── CameraComp.h
│   │   │   │   └── LightComp.h
│   │   │   └── Assets/
│   │   ├── Private/
│   │   └── CMakeLists.txt
│   │
│   ├── Physics/                       ══════════════════════════════╗
│   │   ├── Public/                    ║ SparklePhysics.dll          ║
│   │   │   ├── Physics.h              ║ ────────────────────────    ║
│   │   │   ├── PhysicsWorld.h         ║ • Physics simulation        ║
│   │   │   ├── RigidBody.h            ║ • Collision detection       ║
│   │   │   ├── Colliders/             ║ • Raycasting                ║
│   │   │   │   ├── BoxCollider.h      ║ • Joints                    ║
│   │   │   │   ├── SphereCollider.h   ║                             ║
│   │   │   │   └── MeshCollider.h     ║ Dependencies: Core, Engine  ║
│   │   │   └── PhysicsMaterial.h      ║ Backend: PhysX or Jolt      ║
│   │   ├── Private/                   ╚══════════════════════════════╝
│   │   │   ├── PhysX/
│   │   │   └── Jolt/
│   │   └── CMakeLists.txt
│   │
│   ├── Animation/                     ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleAnimation.dll        ║
│   │   │   ├── Animation.h            ║ ────────────────────────    ║
│   │   │   ├── Skeleton.h             ║ • Skeletal animation        ║
│   │   │   ├── AnimationClip.h        ║ • Animation blending        ║
│   │   │   ├── AnimGraph/             ║ • State machines            ║
│   │   │   │   ├── AnimGraph.h        ║ • IK solvers                ║
│   │   │   │   ├── BlendNode.h        ║ • Root motion               ║
│   │   │   │   └── StateMachine.h     ║                             ║
│   │   │   └── IK/                    ║ Dependencies: Core, Engine  ║
│   │   │       └── IKSolver.h         ║   Renderer                  ║
│   │   ├── Private/                   ╚══════════════════════════════╝
│   │   └── CMakeLists.txt
│   │
│   ├── Audio/                         ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleAudio.dll            ║
│   │   │   ├── Audio.h                ║ ────────────────────────    ║
│   │   │   ├── AudioSystem.h          ║ • 3D spatial audio          ║
│   │   │   ├── AudioSource.h          ║ • Sound mixing              ║
│   │   │   ├── AudioListener.h        ║ • Streaming                 ║
│   │   │   └── AudioClip.h            ║ • Effects                   ║
│   │   ├── Private/                   ║                             ║
│   │   │   ├── FMOD/                  ║ Dependencies: Core, Engine  ║
│   │   │   └── Wwise/                 ║ Backend: FMOD or Wwise      ║
│   │   └── CMakeLists.txt             ╚══════════════════════════════╝
│   │
│   ├── AI/                            ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleAI.dll               ║
│   │   │   ├── AI.h                   ║ ────────────────────────    ║
│   │   │   ├── Navigation/            ║ • Pathfinding               ║
│   │   │   │   ├── NavMesh.h          ║ • Behavior Trees            ║
│   │   │   │   └── NavAgent.h         ║ • Blackboards               ║
│   │   │   ├── BehaviorTree/          ║ • Perception                ║
│   │   │   │   ├── BehaviorTree.h     ║                             ║
│   │   │   │   ├── BTNode.h           ║ Dependencies: Core, Engine  ║
│   │   │   │   └── Blackboard.h       ║   Physics                   ║
│   │   │   └── Perception/            ╚══════════════════════════════╝
│   │   ├── Private/
│   │   │   └── Recast/
│   │   └── CMakeLists.txt
│   │
│   ├── Networking/                    ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleNetworking.dll       ║
│   │   │   ├── Networking.h           ║ ────────────────────────    ║
│   │   │   ├── NetworkManager.h       ║ • Client-Server             ║
│   │   │   ├── Replication/           ║ • Replication               ║
│   │   │   │   ├── NetObject.h        ║ • RPCs                      ║
│   │   │   │   └── NetProperty.h      ║ • Prediction                ║
│   │   │   └── Transport/             ║                             ║
│   │   │       └── Socket.h           ║ Dependencies: Core, Engine  ║
│   │   ├── Private/                   ╚══════════════════════════════╝
│   │   └── CMakeLists.txt
│   │
│   ├── UI/                            ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleUI.dll               ║
│   │   │   ├── UI.h                   ║ ────────────────────────    ║
│   │   │   ├── Canvas.h               ║ • Widget system             ║
│   │   │   ├── Widgets/               ║ • Layout                    ║
│   │   │   │   ├── Widget.h           ║ • Data binding              ║
│   │   │   │   ├── Button.h           ║ • Styles                    ║
│   │   │   │   ├── Text.h             ║                             ║
│   │   │   │   └── Panel.h            ║ Dependencies: Core,Renderer ║
│   │   │   └── Layout/                ║   Platform                  ║
│   │   ├── Private/                   ╚══════════════════════════════╝
│   │   └── CMakeLists.txt
│   │
│   ├── Scripting/                     ══════════════════════════════╗
│   │   ├── Public/                    ║ SparkleScripting.dll        ║
│   │   │   ├── Scripting.h            ║ ────────────────────────    ║
│   │   │   ├── ScriptEngine.h         ║ • Script VM                 ║
│   │   │   ├── ScriptClass.h          ║ • C++ bindings              ║
│   │   │   └── NativeBindings.h       ║ • Hot reload                ║
│   │   ├── Private/                   ║                             ║
│   │   │   ├── Lua/                   ║ Dependencies: Core, Engine  ║
│   │   │   └── Mono/                  ║ Backend: Lua or C#          ║
│   │   └── CMakeLists.txt             ╚══════════════════════════════╝
│   │
│   └── Editor/                        ══════════════════════════════╗
│       ├── Public/                    ║ SparkleEditor.dll           ║
│       │   ├── Editor.h               ║ ────────────────────────    ║
│       │   ├── EditorEngine.h         ║ • Editor application        ║
│       │   ├── Windows/               ║ • Dockable windows          ║
│       │   ├── Commands/              ║ • Undo/Redo                 ║
│       │   └── Selection/             ║ • Selection                 ║
│       ├── Private/                   ║                             ║
│       │   └── Tools/                 ║ Dependencies: ALL DLLs      ║
│       └── CMakeLists.txt             ╚══════════════════════════════╝
│
├── Programs/
│   ├── Editor/                        # SparkleEditor.exe
│   ├── Runtime/                       # SparkleGame.exe
│   └── Tools/
│       ├── ShaderCompiler/
│       ├── AssetCooker/
│       └── HeaderTool/
│
├── Projects/
│   └── MyGame/
│       ├── Source/                    # Game-specific code (DLL)
│       ├── Content/                   # Assets
│       └── Config/                    # Settings
│
└── ThirdParty/
    └── ...
```

---

## Key Concepts

### 1. Public/Private Header Split

Each module has explicit API boundaries:

```
Module/
├── Public/           # Exported headers (API)
│   └── Feature.h     # Other modules can include
└── Private/          # Internal implementation
    └── Feature.cpp   # Hidden from other modules
```

### 2. Export Macros

Every public class needs export decoration:

```cpp
// Core/Public/CoreAPI.h
#pragma once

#ifdef SPARKLE_CORE_EXPORTS
    #define SPARKLE_CORE_API __declspec(dllexport)
#else
    #define SPARKLE_CORE_API __declspec(dllimport)
#endif

// Usage in headers:
class SPARKLE_CORE_API Vector3
{
    float X, Y, Z;
};
```

### 3. Module Dependencies

Strict layering prevents circular dependencies:

```
✅ Renderer includes RHI headers
✅ Engine includes Renderer headers
❌ RHI cannot include Renderer headers
❌ Core cannot include anything above it
```

---

## Build Impact Analysis

```
┌─────────────────────┬───────────────────────────────────────────────────────┐
│ File Changed        │ Modules Rebuilt                                       │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Core/Math/Vector.h  │ ████████████████████████████████ ALL (12 DLLs)       │
│                     │ ⚠️  Core rarely changes - acceptable                  │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ RHI/RHIDevice.h     │ ████████████████████ Renderer + above (8 DLLs)       │
│                     │ ⚠️  RHI interface stable after design                 │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Renderer/Material.h │ ████████████ Engine, Editor, Game (4 DLLs)           │
│                     │ ℹ️  Renderer changes are frequent - good isolation    │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Physics/RigidBody.h │ ██ Physics, Game (2 DLLs)                            │
│                     │ ✅ Excellent isolation                                │
├─────────────────────┼───────────────────────────────────────────────────────┤
│ Game/Player.cpp     │ █ Game only (1 DLL)                                  │
│                     │ ✅ Perfect - game code isolated                       │
└─────────────────────┴───────────────────────────────────────────────────────┘
```

---

## Advantages

### ⚡ Fast Incremental Builds

```
Change Physics.cpp:
• Compile: 2 files (3 sec)
• Link: 1 DLL (5 sec)
• Total: ~10 seconds

vs Monolithic:
• Compile: 2 files (3 sec)
• Link: 1 EXE (90 sec)
• Total: ~95 seconds
```

### 🎯 Clear Ownership

- Each DLL has single responsibility
- Clear public API
- Team members can work in parallel
- Easy to assign maintainers

### 🔌 Plugin Potential

- DLLs can be loaded on demand
- Can unload to free memory
- Potential for runtime replacement

### 📊 Memory Isolation

- Each DLL has own static data
- Easier to track allocations per-module
- Simpler leak detection

---

## Disadvantages

### ⚠️ DLL Boundary Overhead

- Virtual calls at boundaries
- Can't inline across DLLs
- Export/import macro boilerplate

```cpp
// Every public class needs:
class SPARKLE_PHYSICS_API RigidBody { };
```

### 🔧 Complex Setup

- Export macros everywhere
- Complex CMake configuration
- 12+ CMakeLists.txt files

### 🐛 Debugging Challenges

- Stack traces across DLLs
- PDB files for each DLL
- Breakpoints can be tricky

### 📦 Distribution

- Ship 12+ DLL files
- Version compatibility concerns
- Potential "DLL Hell"

---

## CMake Example

```cmake
# Engine/Core/CMakeLists.txt
set(CORE_SOURCES
    Private/Math/Vector.cpp
    Private/Memory/Allocator.cpp
    # ...
)

if(BUILD_SHARED_LIBS)
    add_library(SparkleCore SHARED ${CORE_SOURCES})
    target_compile_definitions(SparkleCore PRIVATE SPARKLE_CORE_EXPORTS)
else()
    add_library(SparkleCore STATIC ${CORE_SOURCES})
    target_compile_definitions(SparkleCore PUBLIC SPARKLE_CORE_STATIC)
endif()

target_include_directories(SparkleCore
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

# Engine/Renderer/CMakeLists.txt
add_library(SparkleRenderer SHARED ${RENDERER_SOURCES})
target_link_libraries(SparkleRenderer
    PUBLIC SparkleCore
    PUBLIC SparkleRHI
)
```

---

## When to Use This Architecture

✅ **Good for:**
- Teams of 10-100 developers
- Projects with long development cycles
- When build time is critical
- Interview demonstrations ("Unreal-style")

❌ **Avoid if:**
- Solo developer or small team
- Simple projects
- Need maximum runtime performance
- Prefer simpler debugging

---

## Migration from Current State

**Timeline: 2-4 weeks**

```
Week 1:
├── Extract SparkleCore.dll (Math, Events, Log, Memory)
└── Set up export macros

Week 2:
├── Extract SparklePlatform.dll (Window, Input)
└── Extract SparkleRHI.dll (D3D12)

Week 3-4:
├── Extract SparkleRenderer.dll
├── Extract SparkleEngine.dll
└── Verify all builds work
```
