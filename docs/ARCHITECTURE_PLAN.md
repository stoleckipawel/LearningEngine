# Engine Module Architecture — Re-Architecture Plan

**Created:** January 31, 2026  
**Status:** Planning Phase

---

## Current State Analysis

Current structure:

```
LearningEngine/
├── engine/
│   └── src/
│       ├── App.cpp/h                    # Application entry + lifecycle
│       ├── Renderer/                    # Rendering (mixed concerns)
│       ├── RHI/                         # Render Hardware Interface
│       │   ├── D3D12/                   # D3D12 backend
│       │   └── Vulkan/                  # Vulkan placeholder
│       ├── Platform/                    # Window, Input
│       ├── Scene/                       # Scene, Mesh, Camera
│       ├── GameEngine/                  # Camera controller
│       ├── Core/                        # Events, Utils, Allocators
│       ├── Assets/                      # Asset system
│       ├── UI/                          # ImGui integration
│       └── Shaders/                     # HLSL shaders
├── external/                            # Third-party libs
└── assets/                              # Runtime assets
```

### Current Problems

| Issue | Impact |
|-------|--------|
| **Flat structure** | No clear module boundaries for DLL extraction |
| **Mixed responsibilities** | `engine/src` contains runtime, tools, and editor code |
| **No separation between Engine/Game/Editor** | Can't build game without editor |
| **Shaders inside engine source** | Should be assets, not compiled code |
| **No explicit public/private API separation** | Everything is accessible |
| **Platform code scattered** | Window in Platform, RHI in separate folder |

---

## Target Architecture

Following **Unreal**, **Godot**, **O3DE (Amazon)**, and **The Forge** patterns:

```
LearningEngine/
│
├── Engine/                          # CORE ENGINE (DLL: Engine.dll)
│   ├── Public/                      # Public API headers (exported)
│   │   ├── Core/
│   │   ├── Math/
│   │   ├── Memory/
│   │   ├── Containers/
│   │   └── Engine.h                 # Master include
│   ├── Private/                     # Implementation (not exported)
│   │   ├── Core/
│   │   ├── Math/
│   │   └── Memory/
│   └── Engine.Build.cs              # Build configuration (future)
│
├── RHI/                             # RHI ABSTRACTION (DLL: RHI.dll)
│   ├── Public/
│   │   ├── IRhi.h
│   │   ├── RHIResources.h
│   │   ├── RHICommandList.h
│   │   └── RHI.h
│   └── Private/
│       └── RHI.cpp
│
├── RHI_D3D12/                       # D3D12 BACKEND (DLL: RHI_D3D12.dll)
│   ├── Public/
│   │   └── D3D12RhiModule.h         # Module entry point only
│   └── Private/
│       ├── D3D12Rhi.cpp/h
│       ├── D3D12Device.cpp/h
│       ├── D3D12SwapChain.cpp/h
│       ├── D3D12CommandList.cpp/h
│       ├── D3D12DescriptorHeap.cpp/h
│       ├── D3D12PipelineState.cpp/h
│       └── D3D12Resources.cpp/h
│
├── RHI_Vulkan/                      # VULKAN BACKEND (DLL: RHI_Vulkan.dll)
│   ├── Public/
│   │   └── VulkanRhiModule.h
│   └── Private/
│       └── ...
│
├── Renderer/                        # HIGH-LEVEL RENDERER (DLL: Renderer.dll)
│   ├── Public/
│   │   ├── Renderer.h
│   │   ├── RenderGraph.h
│   │   ├── RenderPass.h
│   │   ├── SceneRenderer.h
│   │   └── TextureManager.h
│   └── Private/
│       ├── Renderer.cpp
│       ├── RenderGraph.cpp
│       ├── Passes/
│       │   ├── ForwardPass.cpp/h
│       │   ├── ShadowPass.cpp/h
│       │   └── PostProcessPass.cpp/h
│       └── TextureManager.cpp
│
├── Platform/                        # PLATFORM ABSTRACTION (DLL: Platform.dll)
│   ├── Public/
│   │   ├── IWindow.h
│   │   ├── IInputBackend.h
│   │   ├── IPlatform.h
│   │   └── Platform.h
│   └── Private/
│       ├── Windows/
│       │   ├── WindowsWindow.cpp/h
│       │   ├── WindowsInput.cpp/h
│       │   └── WindowsPlatform.cpp/h
│       └── Linux/
│           └── ...
│
├── Scene/                           # SCENE GRAPH (DLL: Scene.dll)
│   ├── Public/
│   │   ├── Scene.h
│   │   ├── Entity.h
│   │   ├── Component.h
│   │   ├── Transform.h
│   │   └── Camera.h
│   └── Private/
│       ├── Scene.cpp
│       ├── Entity.cpp
│       └── ComponentRegistry.cpp
│
├── Assets/                          # ASSET SYSTEM (DLL: Assets.dll)
│   ├── Public/
│   │   ├── AssetManager.h
│   │   ├── AssetHandle.h
│   │   ├── IAssetLoader.h
│   │   └── AssetTypes.h
│   └── Private/
│       ├── AssetManager.cpp
│       ├── Loaders/
│       │   ├── TextureLoader.cpp/h
│       │   ├── MeshLoader.cpp/h
│       │   └── ShaderLoader.cpp/h
│       └── AssetDatabase.cpp
│
├── UI/                              # UI SYSTEM (DLL: UI.dll)
│   ├── Public/
│   │   ├── UISystem.h
│   │   ├── UIPanel.h
│   │   └── UIRenderer.h
│   └── Private/
│       ├── ImGui/
│       │   ├── ImGuiBackend.cpp/h
│       │   └── ImGuiRenderer.cpp/h
│       └── Panels/
│           ├── StatsPanel.cpp/h
│           └── ViewModePanel.cpp/h
│
├── Application/                     # APPLICATION FRAMEWORK (DLL: Application.dll)
│   ├── Public/
│   │   ├── Application.h
│   │   ├── IEngineSystem.h
│   │   ├── SystemRegistry.h
│   │   └── EngineLoop.h
│   └── Private/
│       ├── Application.cpp
│       ├── EngineLoop.cpp
│       └── SystemRegistry.cpp
│
├── Editor/                          # EDITOR (EXE: Editor.exe)
│   ├── Public/
│   │   └── Editor.h
│   └── Private/
│       ├── EditorApplication.cpp/h
│       ├── Panels/
│       │   ├── SceneHierarchy.cpp/h
│       │   ├── Inspector.cpp/h
│       │   ├── AssetBrowser.cpp/h
│       │   └── Viewport.cpp/h
│       └── Tools/
│           └── ...
│
├── Runtime/                         # GAME RUNTIME (EXE: Game.exe)
│   ├── Public/
│   │   └── GameApplication.h
│   └── Private/
│       ├── main.cpp
│       └── GameApplication.cpp
│
├── Shaders/                         # SHADER SOURCE (NOT in engine)
│   ├── Forward/
│   │   ├── ForwardLitVS.hlsl
│   │   └── ForwardLitPS.hlsl
│   ├── Shadow/
│   ├── PostProcess/
│   └── Common/
│       ├── Common.hlsli
│       └── Lighting.hlsli
│
├── Content/                         # RUNTIME ASSETS
│   ├── Textures/
│   ├── Meshes/
│   ├── Materials/
│   └── Levels/
│
├── ThirdParty/                      # EXTERNAL DEPENDENCIES
│   ├── imgui/
│   ├── DirectXMath/
│   ├── stb/
│   └── ...
│
├── Tools/                           # OFFLINE TOOLS
│   ├── ShaderCompiler/
│   ├── AssetCooker/
│   └── ProjectGenerator/
│
├── Tests/                           # UNIT & INTEGRATION TESTS
│   ├── Engine/
│   ├── RHI/
│   └── Renderer/
│
└── Build/                           # BUILD SYSTEM
    ├── CMake/
    │   ├── Modules/
    │   │   ├── Engine.cmake
    │   │   ├── RHI.cmake
    │   │   └── ...
    │   └── Toolchain/
    ├── Scripts/
    └── CMakeLists.txt
```

---

## Module Dependency Graph

```
                    ┌─────────────────┐
                    │     Editor      │ (EXE)
                    └────────┬────────┘
                             │
          ┌──────────────────┼──────────────────┐
          │                  │                  │
          ▼                  ▼                  ▼
    ┌───────────┐     ┌───────────┐     ┌───────────┐
    │    UI     │     │  Renderer │     │   Scene   │
    └─────┬─────┘     └─────┬─────┘     └─────┬─────┘
          │                 │                 │
          │           ┌─────┴─────┐           │
          │           ▼           ▼           │
          │     ┌─────────┐ ┌─────────┐       │
          │     │   RHI   │ │ Assets  │       │
          │     └────┬────┘ └────┬────┘       │
          │          │           │            │
          │    ┌─────┴─────┐     │            │
          │    ▼           ▼     │            │
          │ ┌───────┐ ┌───────┐  │            │
          │ │D3D12  │ │Vulkan │  │            │
          │ └───┬───┘ └───┬───┘  │            │
          │     │         │      │            │
          │     └────┬────┘      │            │
          │          │           │            │
          └────┬─────┴───────────┴────────────┘
               │
               ▼
        ┌────────────┐
        │  Platform  │
        └──────┬─────┘
               │
               ▼
        ┌────────────┐
        │   Engine   │ (Core: Math, Memory, Containers, Events)
        └────────────┘
```

---

## Module Breakdown

### Module: Engine (Core)

| Component | Responsibility |
|-----------|---------------|
| `Core/` | Logging, Assertions, TypeTraits |
| `Math/` | Vectors, Matrices, Quaternions, SIMD |
| `Memory/` | Allocators, PoolAllocator, LinearAllocator |
| `Containers/` | FixedVector, RingBuffer, HandleMap |
| `Events/` | Event, Delegate, ScopedEventHandle |
| `Threading/` | JobSystem, TaskGraph (future) |

### Module: RHI

| Component | Responsibility |
|-----------|---------------|
| `IRhi` | Abstract device interface |
| `RHIResources` | Abstract Buffer, Texture, Pipeline |
| `RHICommandList` | Abstract command recording |
| `RHIFence` | GPU synchronization |

### Module: RHI_D3D12

| Component | Responsibility |
|-----------|---------------|
| `D3D12Rhi` | D3D12 device implementation |
| `D3D12CommandList` | Command buffer recording |
| `D3D12DescriptorHeap` | Descriptor management |
| `D3D12PipelineState` | PSO creation/caching |

### Module: Renderer

| Component | Responsibility |
|-----------|---------------|
| `Renderer` | High-level render orchestration |
| `RenderGraph` | Frame graph for pass scheduling |
| `RenderPass` | Base class for render passes |
| `SceneRenderer` | Visibility, culling, batching |
| `TextureManager` | Texture loading/streaming |

### Module: Application

| Component | Responsibility |
|-----------|---------------|
| `Application` | Entry point, system bootstrap |
| `IEngineSystem` | Lifecycle interface |
| `SystemRegistry` | System discovery/ordering |
| `EngineLoop` | Frame tick orchestration |

---

## Implementation Plan

### Phase 1: Foundation (Week 1)

| Task | Priority | Status |
|------|----------|--------|
| Create new folder structure | P0 | ⬜ |
| Add Public/Private separation | P0 | ⬜ |
| Create `IEngineSystem` interface | P0 | ✅ |
| Move Core components (Math, Memory, Events) | P0 | ⬜ |
| Update CMake for module structure | P0 | ⬜ |

### Phase 2: RHI Isolation (Week 2)

| Task | Priority | Status |
|------|----------|--------|
| Extract RHI module with Public API only | P0 | ⬜ |
| Move D3D12 to `RHI_D3D12/Private/` | P0 | ⬜ |
| Create module entry points | P1 | ⬜ |
| Validate no D3D12 leakage to public headers | P0 | ⬜ |

### Phase 3: Platform Abstraction (Week 2)

| Task | Priority | Status |
|------|----------|--------|
| Create `IWindow` interface | P0 | ⬜ |
| Move Windows code to `Platform/Private/Windows/` | P0 | ⬜ |
| Stub Linux folder structure | P2 | ⬜ |

### Phase 4: Renderer Extraction (Week 3)

| Task | Priority | Status |
|------|----------|--------|
| Extract Renderer module | P1 | ⬜ |
| Create RenderPass base class | P1 | ⬜ |
| Move passes to `Renderer/Private/Passes/` | P1 | ⬜ |
| Extract TextureManager | P1 | ✅ |

### Phase 5: Application Framework (Week 3)

| Task | Priority | Status |
|------|----------|--------|
| Create `Application` module | P1 | ⬜ |
| Implement `SystemRegistry` | P1 | ⬜ |
| Create `EngineLoop` | P1 | ⬜ |
| Migrate `App` to new structure | P1 | ⬜ |

### Phase 6: Editor/Runtime Split (Week 4)

| Task | Priority | Status |
|------|----------|--------|
| Create Editor executable target | P2 | ⬜ |
| Create Runtime executable target | P2 | ⬜ |
| Move current code to Editor | P2 | ⬜ |
| Create minimal Runtime bootstrap | P2 | ⬜ |

---

## Module Export Pattern

Each module will use a standard export macro pattern:

```cpp
// Engine/Public/Core/EngineAPI.h
#pragma once

#ifdef ENGINE_EXPORTS
    #define ENGINE_API __declspec(dllexport)
#else
    #define ENGINE_API __declspec(dllimport)
#endif

#ifdef ENGINE_STATIC
    #undef ENGINE_API
    #define ENGINE_API
#endif
```

```cpp
// RHI/Public/RHI/RHIAPI.h
#pragma once

#ifdef RHI_EXPORTS
    #define RHI_API __declspec(dllexport)
#else
    #define RHI_API __declspec(dllimport)
#endif
```

---

## CMake Structure

```cmake
# Build/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(LearningEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(LE_BUILD_SHARED "Build as shared libraries" OFF)
option(LE_BUILD_EDITOR "Build editor executable" ON)
option(LE_BUILD_RUNTIME "Build runtime executable" ON)
option(LE_BUILD_TESTS "Build unit tests" ON)

# Module includes
include(Build/CMake/Modules/Engine.cmake)
include(Build/CMake/Modules/RHI.cmake)
include(Build/CMake/Modules/RHI_D3D12.cmake)
include(Build/CMake/Modules/Platform.cmake)
include(Build/CMake/Modules/Assets.cmake)
include(Build/CMake/Modules/Scene.cmake)
include(Build/CMake/Modules/Renderer.cmake)
include(Build/CMake/Modules/UI.cmake)
include(Build/CMake/Modules/Application.cmake)

if(LE_BUILD_EDITOR)
    include(Build/CMake/Modules/Editor.cmake)
endif()

if(LE_BUILD_RUNTIME)
    include(Build/CMake/Modules/Runtime.cmake)
endif()

if(LE_BUILD_TESTS)
    enable_testing()
    include(Build/CMake/Modules/Tests.cmake)
endif()
```

```cmake
# Build/CMake/Modules/Engine.cmake
set(ENGINE_SOURCES
    ${CMAKE_SOURCE_DIR}/Engine/Private/Core/Log.cpp
    ${CMAKE_SOURCE_DIR}/Engine/Private/Core/Assert.cpp
    ${CMAKE_SOURCE_DIR}/Engine/Private/Memory/LinearAllocator.cpp
    # ...
)

set(ENGINE_PUBLIC_HEADERS
    ${CMAKE_SOURCE_DIR}/Engine/Public
)

if(LE_BUILD_SHARED)
    add_library(Engine SHARED ${ENGINE_SOURCES})
    target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
else()
    add_library(Engine STATIC ${ENGINE_SOURCES})
    target_compile_definitions(Engine PUBLIC ENGINE_STATIC)
endif()

target_include_directories(Engine
    PUBLIC ${ENGINE_PUBLIC_HEADERS}
    PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Private
)
```

---

## Benefits of This Architecture

| Benefit | Description |
|---------|-------------|
| **DLL-Ready** | Public/Private split enables clean DLL boundaries |
| **Fast Iteration** | Change D3D12 backend without rebuilding Engine |
| **Testable** | Mock RHI for unit tests |
| **Platform Portable** | Swap Windows for Linux without touching Renderer |
| **Editor/Game Split** | Ship game without editor code |
| **Clear Ownership** | Each module has explicit responsibility |
| **Scalable** | Add RHI_Vulkan, RHI_Metal without touching existing code |

---

## References

- **Unreal Engine** — Module system, Public/Private separation
- **Godot Engine** — Scene graph, editor architecture
- **O3DE (Amazon)** — Gem system, modular architecture
- **The Forge** — RHI abstraction patterns
- **Frostbite** — Rendering architecture (GDC talks)
