# LearningEngine

## Overview

LearningEngine is a modular DirectX 12 rendering engine built for learning and experimentation. The codebase follows industry-standard patterns inspired by AAA engines (Unreal, Frostbite, id Tech) with a focus on clean architecture, performance, and extensibility.

**Key Features:**
- Modern C++20 codebase with zero-allocation logging and efficient resource management
- Modular architecture separating Core, Platform, RHI, Renderer, Resources, Scene, and UI
- D3D12-based RHI with descriptor heap management, upload buffers, and PSO abstraction
- Compile-time and runtime log level filtering
- ImGui integration for debug UI
- Clean public API / private implementation separation

## Prerequisites

- **CMake** (≥ 3.20)
- **Visual Studio 2022** (or compatible)
- **DirectX 12 SDK / Windows 10 SDK**
- **Windows platform**
- **Clang** (optional; if not available, build will automatically fall back to MSVC)

## Quick Start

1. **Check Dependencies**
	```
	CheckDependencies.bat
	```
	Verifies CMake, Clang, and MSBuild are available. Logs missing tools.

2. **Generate Solution**
	```
	BuildSolution.bat
	```
	Cleans, checks dependencies, and generates Visual Studio solution files using CMake.

3. **Build Sample Projects**
	```
	BuildSamplesDebug.bat
	BuildSamplesRelease.bat
	BuildSamples.bat Debug
	BuildSamples.bat Release
	```
	Builds all sample projects for Debug or Release. Output executables are in `bin/Debug` or `bin/Release`.

4. **Manual Build (Visual Studio)**
	Open `build/Playground.sln` in Visual Studio. Build using Debug/Release. Default startup project: `ExampleD3D12`.

## Batch Script Reference

| Script                     | Purpose                                                        |
|----------------------------|----------------------------------------------------------------|
| CheckDependencies.bat      | Check for CMake, Clang, MSBuild                                |
| CleanIntermediateFiles.bat | Clean build, bin, obj, .vs, and temp files                     |
| BuildSolution.bat          | Clean, check dependencies, generate solution                   |
| BuildSamples.bat           | Build all sample projects for given config (Debug/Release)      |
| BuildSamplesDebug.bat      | Build all sample projects in Debug (calls BuildSamples.bat)     |
| BuildSamplesRelease.bat    | Build all sample projects in Release (calls BuildSamples.bat)   |

## Directory Structure

```
LearningEngine/
├── engine/                           # Engine library
│   ├── include/                      # Public API headers (exposed to samples/games)
│   │   ├── App.h                     # Application framework
│   │   ├── EngineConfig.h            # Engine configuration
│   │   └── Log.h                     # Logging facade
│   │
│   ├── src/                          # Private implementation
│   │   ├── Core/                     # Foundation (logging, timing, memory, debug)
│   │   │   ├── Log.cpp
│   │   │   ├── Timer.*
│   │   │   ├── LinearAllocator.h
│   │   │   ├── DebugUtils.h
│   │   │   └── PCH.*                 # Precompiled header
│   │   │
│   │   ├── Platform/                 # OS abstraction (window, input)
│   │   │   └── Window.*
│   │   │
│   │   ├── RHI/                      # Render Hardware Interface
│   │   │   │
│   │   │   ├── D3D12/                # DirectX 12 Backend ¤¤¤
│   │   │   │   ├── RHI.*             # Device, command queue, fence
│   │   │   │   ├── SwapChain.*
│   │   │   │   ├── DebugLayer.*
│   │   │   │   ├── Descriptors/      # Descriptor heaps, handles, allocators
│   │   │   │   ├── Pipeline/         # PSO, root signatures, samplers
│   │   │   │   ├── Resources/        # Buffers, depth targets, constants
│   │   │   │   └── Shaders/          # HLSL/DXIL compilation
│   │   │   │
│   │   │   └── Vulkan/               # ¤¤¤ Vulkan Backend (placeholder) ¤¤¤
│   │   │
│   │   ├── Renderer/                 # High-level rendering (API-agnostic)
│   │   │   ├── Renderer.*
│   │   │   └── Camera.*
│   │   │
│   │   ├── Resources/                # Asset loading
│   │   │   ├── Texture.*
│   │   │   ├── TextureLoader.*
│   │   │   └── AssetPathResolver.h
│   │   │
│   │   ├── Scene/                    # Primitives, geometry
│   │   │   ├── Primitive.*
│   │   │   ├── PrimitiveBox.*
│   │   │   ├── PrimitivePlane.*
│   │   │   └── PrimitiveFactory.*
│   │   │
│   │   ├── UI/                       # ImGui integration
│   │   │   └── UI.*
│   │   │
│   │   └── App.cpp                   # Application lifecycle
│   │
│   ├── assets/                       # Engine assets
│   │   ├── shaders/                  # HLSL shaders (D3D12), GLSL/SPIR-V (Vulkan)
│   │   └── textures/
│   │
│   └── third_party/                  # External dependencies
│       ├── d3dx12.h
│       └── imgui/
│
├── samples/                          # Sample projects using the engine
│   └── ExampleD3D12/
│       └── src/
│           └── main.cpp
│
├── build/                            # Generated build files (gitignored)
│   └── Playground.sln                # Visual Studio solution
│
└── bin/                              # Compiled binaries (gitignored)
    ├── Debug/
    └── Release/
```

### Module Overview

| Module | Responsibility |
|--------|----------------|
| **Core** | Logging, timing, memory utilities — no graphics dependencies |
| **Platform** | OS abstraction: window creation, message loop |
| **RHI** | D3D12 wrappers: device, command queues, descriptors, buffers, PSOs |
| **Renderer** | High-level rendering: camera, render passes |
| **Resources** | Asset loading: textures, (future: meshes, materials) |
| **Scene** | Geometry primitives, (future: scene graph, transforms) |
| **UI** | ImGui integration for debug UI |

## Troubleshooting

- **Missing dependencies:** Ensure Windows 10 SDK and DirectX 12 libraries are installed.
  ```
  CheckDependencies.bat
  ```

- **Build errors or workspace reset:**
  ```
  CleanIntermediateFiles.bat
  ```
  Removes all intermediate, build, and temporary files. After cleaning, run `BuildSolution.bat` to regenerate.

- **Include errors after folder changes:** Re-run CMake to update include paths:
  ```
  cd build && cmake ..
  ```

## Architecture Notes

The engine uses a layered architecture where higher-level modules depend on lower-level ones:

```
┌─────────────────────────────────────────────────┐
│                   Samples                       │
├─────────────────────────────────────────────────┤
│         Renderer    │    Scene    │     UI      │
├─────────────────────┴─────────────┴─────────────┤
│                    Resources                    │
├─────────────────────────────────────────────────┤
│                      RHI                        │
├─────────────────────────────────────────────────┤
│           Platform          │       Core        │
└─────────────────────────────┴───────────────────┘
```

- **Core** has no dependencies — can be used in tools, tests, or headless builds
- **RHI** abstracts D3D12; future Vulkan backend would be added here
- **Renderer** uses RHI but doesn't know about D3D12 specifics
- **Scene/Resources** are independent and can be extended without touching rendering

## License

MIT License. See `LICENSE.txt` for details.