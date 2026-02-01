# SparkleEngine - Layered DLL Architecture Integration Report

**Date:** February 1, 2026  
**Architecture:** Option A - Layered DLL (Unreal-style)  
**Status:** ✅ Core modules implemented and building | ⚠️ Significant coupling issues identified

---

## Executive Summary

The SparkleEngine has been restructured from a monolithic static library to a **Layered DLL Architecture** following Unreal Engine's proven design patterns. All 6 core modules now compile successfully with per-module precompiled headers and configuration files.

### Current State
- ✅ **All modules compiling** - Core, Platform, RHI, Renderer, GameFramework, UI
- ✅ **Per-module configuration** - RHIConfig.h, PlatformConfig.h, UIConfig.h (decoupled from monolithic EngineConfig.h)
- ✅ **Per-module PCH** - Each module has its own precompiled header for fast builds
- ⚠️ **Significant coupling** - Layer violations exist that need refactoring

### Key Benefits Achieved
- ✅ **Fast incremental builds** - Change one module, only rebuild that module + dependents
- ✅ **Clear API boundaries** - Public/Private header split enforces encapsulation  
- ✅ **Modular development** - Teams can work on different modules in parallel
- ✅ **Per-module configuration** - Each module owns its compile-time settings
- ⚠️ **Future DLL support** - Ready to switch, but coupling issues must be resolved first

---

## Current Architecture (Actual State)

```
┌─────────────────────────────────────────────────────────────┐
│                    SparkleUI.dll                            │  ← Widget system, ImGui
│              Depends: Core, Platform, Renderer              │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                SparkleGameFramework.dll                     │  ← Scene, Camera, Primitives
│            Depends: Core, Platform, RHI, Renderer           │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                   SparkleRenderer.dll                       │  ← RenderCamera, TextureManager
│              Depends: Core, Platform, RHI                   │
│     ⚠️ Also includes: GameFramework (Scene, Camera), UI     │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                     SparkleRHI.dll                          │  ← D3D12 backend
│              Depends: Core, Platform                        │
│   ⚠️ Also includes: Renderer, GameFramework, UI             │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                   SparklePlatform.dll                       │  ← Window, Input
│              Depends: Core                                  │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    SparkleCore.dll                          │  ← Math, Events, Logging
│              Depends: None                                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔴 Critical: Coupling Analysis

### Layer Violations Identified

The following dependencies violate proper layering and need refactoring:

| Source Module | Includes From | Files Affected | Severity |
|---------------|---------------|----------------|----------|
| **RHI** | Renderer (`DepthConvention.h`) | D3D12DepthStencil.cpp, D3D12PipelineState.cpp | 🔴 High |
| **RHI** | Renderer (`RenderCamera.h`) | D3D12ConstantBufferManager.h/cpp | 🔴 High |
| **RHI** | UI (`UI.h`) | D3D12ConstantBufferManager.h/cpp | 🔴 High |
| **RHI** | GameFramework (`AssetSystem.h`) | DxcShaderCompiler.cpp, TextureLoader.cpp | 🟡 Medium |
| **Renderer** | GameFramework (`Scene.h`, `GameCamera.h`) | Renderer.h/cpp | 🟡 Medium |
| **Renderer** | UI (`UI.h`) | Renderer.cpp | 🟡 Medium |

### Current Problematic Flow

```
     RHI ◄───────────────────────────► Renderer
      │   (Uses DepthConvention,       (Uses RHI ✓)
      │    RenderCamera from Renderer)
      │
      ├──────────────────────────────► UI
      │   (Uses UI.h for CB updates)   (Should depend on RHI, not reverse)
      │
      └──────────────────────────────► GameFramework
          (Uses AssetSystem for paths)  (Should depend on RHI, not reverse)
```

---

## 🔧 Decoupling Plan (Future Work)

### Phase 1: Move Shared Types Down

| Item to Move | From | To | Rationale |
|--------------|------|-----|-----------|
| `DepthConvention.h` | Renderer | RHI or Core | Used by RHI pipeline setup |
| Constant buffer data structs | RHI | Core or shared header | Used by multiple modules |

### Phase 2: Introduce Interfaces

| Interface | Location | Purpose |
|-----------|----------|---------|
| `IAssetResolver` | Core | Abstract path resolution for shaders/textures |
| `IFrameProvider` | Core | Get current frame index without UI dependency |
| `ICameraData` | Core | Camera matrices without RenderCamera dependency |

### Phase 3: Invert Dependencies

| Current | Target | Change |
|---------|--------|--------|
| RHI → UI (for frame index) | UI → RHI | UI provides frame context to RHI via interface |
| RHI → Renderer (for camera) | Renderer → RHI | Renderer passes camera data to CB manager |
| RHI → GameFramework (for assets) | Pass paths directly | Caller resolves paths before passing to RHI |

### Phase 4: Clean CMakeLists

Remove these include paths from RHI/CMakeLists.txt:
```cmake
# TO REMOVE:
${CMAKE_CURRENT_SOURCE_DIR}/../Renderer/Public
${CMAKE_CURRENT_SOURCE_DIR}/../UI/Public  
${CMAKE_CURRENT_SOURCE_DIR}/../GameFramework/Public
```

---

## Per-Module Configuration (Implemented)

Each module now has its own configuration header, reducing coupling:

| Module | Config File | Contents |
|--------|-------------|----------|
| **RHI** | `RHIConfig.h` | `ENGINE_GPU_VALIDATION`, `RHISettings::FramesInFlight`, formats, shader model |
| **Platform** | `PlatformConfig.h` | `PlatformSettings::StartFullscreen` |
| **UI** | `UIConfig.h` | `USE_GUI`, `USE_IMGUI_DEMO_WINDOW` |

**Deleted:** `Engine/Core/Public/EngineConfig.h` (replaced by per-module configs)

---

## Module Reference

### Active Modules (Implemented & Building)

| Module | Config File | Purpose | Proper Dependencies | Layer Violations |
|--------|-------------|---------|---------------------|------------------|
| **Core** | - | Math, Events, Diagnostics, Time | None | None ✅ |
| **Platform** | `PlatformConfig.h` | Window, Input | Core | None ✅ |
| **RHI** | `RHIConfig.h` | D3D12 backend | Core, Platform | ⚠️ Renderer, UI, GameFramework |
| **Renderer** | - | RenderCamera, materials | Core, Platform, RHI | ⚠️ GameFramework, UI |
| **GameFramework** | - | Scene, Camera, Primitives, Assets | Core, Platform, RHI, Renderer | None ✅ |
| **UI** | `UIConfig.h` | Widget system, ImGui | Core, Platform, Renderer | None ✅ |

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

Projects must explicitly link only the modules they need. This keeps builds fast and dependencies clear.

```cmake
# CMakeLists.txt for a typical game project
target_link_libraries(MyGame PRIVATE
    SparkleCore           # Math, Events, Logging (required by all)
    SparklePlatform       # Window, Input
    SparkleRHI            # D3D12 backend
    SparkleRenderer       # High-level rendering
    SparkleGameFramework  # Scene, Camera, Primitives
    SparkleUI             # ImGui integration
)

# For a headless tool (no rendering)
target_link_libraries(MyTool PRIVATE
    SparkleCore           # Only what you need
)

# For a minimal renderer (no game systems)
target_link_libraries(MyRenderer PRIVATE
    SparkleCore
    SparklePlatform
    SparkleRHI
    SparkleRenderer
)
```

**Note:** There is no `SparkleEngineAll` convenience target. Explicit linking is required to maintain modularity and fast incremental builds.

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
| Per-module configuration | Each module owns its settings, reduced coupling |
| Industry standard | Follows Unreal Engine patterns |

### Disadvantages ⚠️

| Disadvantage | Mitigation |
|--------------|------------|
| More complex setup | Templates and documentation provided |
| Export macro boilerplate | Copy existing patterns |
| Can't inline across modules | Keep hot code within modules |
| More files to manage | IDE support, clear naming |
| **Layer violations exist** | Decoupling session planned (see below) |

---

## Next Steps

### Immediate (Decoupling Session Required)

1. **Move `DepthConvention.h`** - From Renderer to RHI (or Core)
2. **Extract CB data structs** - Move `PerFrameConstantBufferData`, `PerViewConstantBufferData` to shared location
3. **Create `IAssetResolver`** - Abstract interface in Core for path resolution
4. **Remove upward dependencies** - RHI should not include Renderer/UI/GameFramework headers

### After Decoupling

1. **Test DLL builds** - Verify `SPARKLE_BUILD_SHARED=ON` works correctly
2. **Add export macros** - Ensure all public classes have `SPARKLE_*_API`
3. **Implement placeholders** - Physics, Audio, AI as needed

---

## Decoupling Session Checklist

```
[ ] Move DepthConvention.h to RHI/Public
[ ] Create IAssetResolver interface in Core
[ ] Create ICameraData interface in Core  
[ ] Refactor D3D12ConstantBufferManager to use interfaces
[ ] Refactor TextureLoader to receive resolved paths
[ ] Refactor DxcShaderCompiler to receive resolved paths
[ ] Remove Renderer/UI/GameFramework includes from RHI CMakeLists.txt
[ ] Remove GameFramework/UI includes from Renderer CMakeLists.txt
[ ] Verify clean build with proper layering
[ ] Test DLL mode
```

---

*This architecture provides a solid foundation for a professional game engine. The coupling issues are documented and a clear plan exists for resolution.*
