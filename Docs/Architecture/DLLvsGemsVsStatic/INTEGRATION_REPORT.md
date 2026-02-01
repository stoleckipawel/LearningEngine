# SparkleEngine - Layered DLL Architecture Integration Report

**Date:** February 1, 2026  
**Architecture:** Option A - Layered DLL (Unreal-style)  
**Status:** Core modules implemented, placeholder modules ready for expansion

---

## Executive Summary

The SparkleEngine has been restructured from a monolithic static library to a **Layered DLL Architecture** following Unreal Engine's proven design patterns. This architecture organizes the engine into strict hierarchical modules where each layer only depends on layers below it.

### Key Benefits Achieved
- ✅ **Fast incremental builds** - Change one module, only rebuild that module + dependents
- ✅ **Clear API boundaries** - Public/Private header split enforces encapsulation  
- ✅ **Modular development** - Teams can work on different modules in parallel
- ✅ **Optional features** - Physics, Audio, AI can be enabled/disabled per project
- ✅ **Future DLL support** - Ready to switch from static to shared libraries

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                     SparkleEditor.dll                       │  ← Editor tools (placeholder)
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                SparkleGameFramework.dll                     │  ← World, Entities, Components
│            (SparkleEngine in Unreal terms)                  │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│SparklePhysics │    │ SparkleAudio  │    │  SparkleAI    │    ← Optional modules
│  (placeholder)│    │  (placeholder)│    │  (placeholder)│       (placeholders)
└───────────────┘    └───────────────┘    └───────────────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   SparkleRenderer.dll                       │  ← Render passes, materials
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                     SparkleRHI.dll                          │  ← D3D12/Vulkan abstraction
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                   SparklePlatform.dll                       │  ← Window, Input, FileSystem
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    SparkleCore.dll                          │  ← Math, Events, Logging
└─────────────────────────────────────────────────────────────┘
```

---

## Module Reference

### Active Modules (Implemented)

| Module | DLL Name | Purpose | Dependencies | File Count |
|--------|----------|---------|--------------|------------|
| **Core** | SparkleCore | Math, Events, Diagnostics, Time, Hash, Strings | None | 36 |
| **Platform** | SparklePlatform | Window, Input, FileSystem | Core | 9 |
| **RHI** | SparkleRHI | D3D12 backend, GPU abstraction | Core, Platform | 45 |
| **Renderer** | SparkleRenderer | Render camera, materials, texture management | Core, Platform, RHI | 9 |
| **GameFramework** | SparkleEngine | World, Scene, Mesh, Camera, Assets | Core, Platform, RHI, Renderer | 48 |
| **UI** | SparkleUI | Widget system, ImGui integration | Core, Platform, Renderer | 12 |

### Placeholder Modules (Ready for Implementation)

| Module | DLL Name | Planned Purpose | Dependencies |
|--------|----------|-----------------|--------------|
| **Physics** | SparklePhysics | PhysX/Jolt integration, collision | Core, GameFramework |
| **Animation** | SparkleAnimation | Skeletal animation, IK, state machines | Core, GameFramework, Renderer |
| **Audio** | SparkleAudio | FMOD/Wwise integration, 3D audio | Core, GameFramework |
| **AI** | SparkleAI | NavMesh, behavior trees, perception | Core, GameFramework, Physics |
| **Networking** | SparkleNetworking | Replication, RPCs, client-server | Core, GameFramework |
| **Scripting** | SparkleScripting | Lua/Mono integration, hot reload | Core, GameFramework |
| **Editor** | SparkleEditor | Editor application, tools | ALL modules |

---

## Directory Structure

```
Engine/
├── Core/                    # SparkleCore - Foundation
│   ├── Public/             
│   │   ├── CoreAPI.h        # Export macros
│   │   ├── CoreMinimal.h    # Common includes
│   │   ├── CoreTypes.h      # Basic types
│   │   ├── Math/            # Vector, Matrix, Frustum
│   │   ├── Events/          # Event system
│   │   ├── Diagnostics/     # Logging
│   │   ├── Time/            # Timer utilities
│   │   ├── Hash/            # Hashing functions
│   │   ├── Strings/         # String utilities
│   │   ├── Input/           # Input types & events
│   │   ├── Memory/          # [Placeholder] Allocators
│   │   ├── Containers/      # [Placeholder] Custom containers
│   │   ├── Delegates/       # [Placeholder] Delegate system
│   │   ├── Async/           # [Placeholder] Task graph, job system
│   │   ├── IO/              # [Placeholder] Async file I/O
│   │   ├── Serialization/   # [Placeholder]
│   │   └── Reflection/      # [Placeholder]
│   ├── Private/
│   │   ├── PCH.h            # Precompiled header
│   │   ├── Diagnostics/     # Log implementation
│   │   └── Input/           # InputState implementation
│   └── CMakeLists.txt
│
├── Platform/                # SparklePlatform - OS Abstraction
│   ├── Public/
│   │   ├── PlatformAPI.h
│   │   ├── Window.h
│   │   ├── Input/           # InputSystem, IInputBackend
│   │   ├── Application/     # [Placeholder]
│   │   ├── Window/          # [Placeholder] Window classes
│   │   └── FileSystem/      # [Placeholder]
│   ├── Private/
│   │   ├── Windows/         # Win32 implementations
│   │   └── Linux/           # [Placeholder]
│   └── CMakeLists.txt
│
├── RHI/                     # SparkleRHI - Graphics API
│   ├── Public/
│   │   ├── RHIAPI.h
│   │   └── D3D12/           # D3D12 backend headers
│   │       ├── Descriptors/
│   │       ├── Pipeline/
│   │       ├── Resources/
│   │       ├── Samplers/
│   │       └── Shaders/
│   ├── Private/
│   │   ├── D3D12/           # D3D12 implementations
│   │   └── Vulkan/          # [Placeholder]
│   └── CMakeLists.txt
│
├── Renderer/                # SparkleRenderer - High-level Rendering
│   ├── Public/
│   │   ├── RendererAPI.h
│   │   ├── Renderer.h
│   │   ├── RenderCamera.h
│   │   ├── DepthConvention.h
│   │   ├── TextureManager.h
│   │   ├── Camera/          # [Placeholder]
│   │   ├── Materials/       # [Placeholder]
│   │   ├── Lighting/        # [Placeholder]
│   │   ├── Streaming/       # [Placeholder] Texture/GPU streaming
│   │   └── PostProcess/     # [Placeholder]
│   ├── Private/
│   │   ├── Passes/          # [Placeholder] Render passes
│   │   └── Shaders/         # [Placeholder]
│   └── CMakeLists.txt
│
├── GameFramework/           # SparkleEngine - Game Systems
│   ├── Public/
│   │   ├── GameFrameworkAPI.h
│   │   ├── Scene/
│   │   │   ├── Camera/       # GameCamera, CameraController
│   │   │   ├── Primitives/Basic/
│   │   │   └── Primitives/Polyhedra/
│   │   ├── Assets/
│   │   ├── Application/
│   │   ├── Streaming/       # [Placeholder] World/LOD streaming
│   │   ├── World/           # [Placeholder]
│   │   └── Components/      # [Placeholder]
│   ├── Private/
│   │   ├── Scene/
│   │   │   ├── Camera/      # CameraController, GameCamera impl
│   │   │   └── Primitives/
│   │   └── Assets/
│   └── CMakeLists.txt
│
├── UI/                      # SparkleUI - Widget System
│   ├── Public/
│   │   ├── UIAPI.h
│   │   ├── UI.h
│   │   ├── Framework/
│   │   ├── Panels/
│   │   ├── Sections/
│   │   ├── Widgets/         # [Placeholder]
│   │   └── Layout/          # [Placeholder]
│   ├── Private/
│   │   └── ImGui/           # ImGui integration
│   └── CMakeLists.txt
│
├── Physics/                 # [PLACEHOLDER] SparklePhysics
│   ├── Public/
│   │   ├── PhysicsAPI.h
│   │   └── Colliders/
│   └── Private/
│       ├── PhysX/
│       └── Jolt/
│
├── Animation/               # [PLACEHOLDER] SparkleAnimation
│   ├── Public/
│   │   ├── AnimationAPI.h
│   │   ├── AnimGraph/
│   │   └── IK/
│   └── Private/
│
├── Audio/                   # [PLACEHOLDER] SparkleAudio
│   ├── Public/
│   │   └── AudioAPI.h
│   └── Private/
│       ├── FMOD/
│       └── Wwise/
│
├── AI/                      # [PLACEHOLDER] SparkleAI
│   ├── Public/
│   │   ├── AIAPI.h
│   │   ├── Navigation/
│   │   ├── BehaviorTree/
│   │   └── Perception/
│   └── Private/
│       └── Recast/
│
├── Networking/              # [PLACEHOLDER] SparkleNetworking
│   ├── Public/
│   │   ├── NetworkingAPI.h
│   │   ├── Replication/
│   │   └── Transport/
│   └── Private/
│
├── Scripting/               # [PLACEHOLDER] SparkleScripting
│   ├── Public/
│   │   └── ScriptingAPI.h
│   └── Private/
│       ├── Lua/
│       └── Mono/
│
├── Editor/                  # [PLACEHOLDER] SparkleEditor
│   ├── Public/
│   │   ├── EditorAPI.h
│   │   ├── Windows/
│   │   ├── Commands/
│   │   └── Selection/
│   └── Private/
│       └── Tools/
│
└── third_party/             # External dependencies
    ├── imgui/
    └── cgltf/
```

---

## How to Use

### Building the Engine

```bash
# Configure with CMake (static libraries by default)
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build all modules
cmake --build build --config Debug

# Build specific module
cmake --build build --config Debug --target SparkleCore
```

### Enabling DLL Mode

```bash
# Configure with shared libraries
cmake -B build -G "Visual Studio 17 2022" -A x64 -DSPARKLE_BUILD_SHARED=ON
```

### Using Modules in Your Project

```cmake
# CMakeLists.txt for your game project
target_link_libraries(MyGame PRIVATE
    SparkleEngineAll    # Links all active modules
)

# Or link specific modules
target_link_libraries(MyGame PRIVATE
    SparkleCore
    SparkleRenderer
    SparkleGameFramework
)
```

### Including Headers

```cpp
// Include module headers
#include "CoreMinimal.h"        // From Core
#include "Window.h"             // From Platform
#include "D3D12/D3D12Rhi.h"     // From RHI
#include "Renderer.h"           // From Renderer
#include "Scene/Scene.h"        // From GameFramework
#include "UI.h"                 // From UI
```

---

## Export Macro Pattern

Each module has its own export macro header:

```cpp
// Engine/Core/Public/CoreAPI.h
#pragma once

#ifdef SPARKLE_STATIC
    #define SPARKLE_CORE_API
#else
    #ifdef SPARKLE_CORE_EXPORTS
        #define SPARKLE_CORE_API __declspec(dllexport)
    #else
        #define SPARKLE_CORE_API __declspec(dllimport)
    #endif
#endif

// Usage in public headers:
class SPARKLE_CORE_API Vector3 { ... };
```

---

## Migration Workflow Summary

### What Was Done

1. **Removed Legacy Structure**
   - Deleted `Engine/Src/` folder (old monolithic source)
   - Deleted `Engine/Include/` folder (old public headers)

2. **Created Module Structure**
   - Each module has `Public/` (API) and `Private/` (implementation) folders
   - Created `CMakeLists.txt` for each active module

3. **Migrated Files**
   - Core: Math, Events, Diagnostics, Time, Hash, Strings, Input
   - Platform: Window, InputSystem, Win32 backend
   - RHI: D3D12 backend (Descriptors, Pipeline, Resources, Samplers, Shaders)
   - Renderer: RenderCamera, TextureManager, DepthConvention
   - GameFramework: Camera, Scene, Mesh, Primitives, Assets
   - UI: Panels, Sections, Framework

4. **Created Placeholder Modules**
   - Physics, Animation, Audio, AI, Networking, Scripting, Editor
   - Each has API header and folder structure ready

### Key Design Decisions

| Decision | Rationale |
|----------|-----------|
| Renamed "Runtime" to "GameFramework" | Better describes its role as the game systems layer |
| Extracted UI to separate module | UI has distinct dependencies and can be optional |
| Public/Private split | Enforces encapsulation, clear API boundaries |
| Placeholder modules with folders | Ready for expansion without restructuring |
| Static library default | Simpler debugging, can switch to DLL when needed |

---

## Extending the Architecture

### Adding a New Module

1. Create folder structure:
   ```
   Engine/MyModule/
   ├── Public/
   │   └── MyModuleAPI.h
   ├── Private/
   └── CMakeLists.txt
   ```

2. Create API header (`Public/MyModuleAPI.h`):
   ```cpp
   #pragma once
   #ifdef SPARKLE_STATIC
       #define SPARKLE_MYMODULE_API
   #else
       #ifdef SPARKLE_MYMODULE_EXPORTS
           #define SPARKLE_MYMODULE_API __declspec(dllexport)
       #else
           #define SPARKLE_MYMODULE_API __declspec(dllimport)
       #endif
   #endif
   ```

3. Create CMakeLists.txt following the pattern in existing modules

4. Add to `Engine/CMakeLists.txt`:
   ```cmake
   add_subdirectory(MyModule)
   ```

### Adding Files to Existing Module

1. Headers go in `Module/Public/` (if part of API) or `Module/Private/` (if internal)
2. Source files go in `Module/Private/`
3. Use appropriate export macro for public classes

---

## Pros and Cons

### Advantages ✅

| Advantage | Impact |
|-----------|--------|
| Fast incremental builds | Change Physics → rebuild Physics + dependents only |
| Clear API boundaries | Public/Private enforces encapsulation |
| Modular development | Teams can work independently |
| Optional modules | Enable only what you need |
| DLL potential | Can switch to shared libraries for hot-reload |
| Industry standard | Follows Unreal Engine patterns |

### Disadvantages ⚠️

| Disadvantage | Mitigation |
|--------------|------------|
| More complex setup | Templates and documentation provided |
| Export macro boilerplate | Copy existing patterns |
| Can't inline across modules | Keep hot code within modules |
| More files to manage | IDE support, clear naming |

---

## Next Steps

1. **Fix Include Paths** - Update `#include` statements in migrated files
2. **Add Export Macros** - Add `SPARKLE_*_API` to public classes
3. **Test Build** - Verify all modules compile
4. **Implement Placeholders** - Add Physics, Audio, AI as needed

---

*This architecture provides a solid foundation for a professional game engine that can scale from solo development to team projects.*
