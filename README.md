# LearningEngine

## Overview

LearningEngine is a modular DirectX 12 rendering engine built for learning and experimentation. 

**Key Features:**
- D3D12 backend.
- DXC-based shader compilation via `DxcShaderCompiler`
- ImGui integration

## Prerequisites

- **CMake** (≥ 3.20)
- **Visual Studio 2022** (or compatible)
- **DirectX 12 SDK / Windows 10 SDK**
- **Windows platform**
- **Clang** (optional; if not available, build will automatically fall back to MSVC)

### Required vs Optional
- **Required:**
	- `CMake` — used to generate build systems and projects.
	- `Visual Studio 2022` / `msbuild` — required for the default Visual Studio solution workflow on Windows.
	- `Windows 10 SDK` / DirectX 12 libraries — runtime and headers for D3D12 development.

- **Optional (recommended for sanitizer or Ninja workflows):**
	- `clang` / `clang++` — required when using sanitizer builds or building with the LLVM toolchain; otherwise the build falls back to MSVC.
	- `Ninja` — recommended generator for fast incremental builds and the sanitizer scripts (scripts configure Ninja + clang automatically).

Why optional tools matter:
- `clang/clang++`: many sanitizers (ASan, UBSan, TSan, MSan) are best supported with LLVM toolchains. Use `clang` when running the `Tools/*Sanitizer*.bat` scripts or when you pass `-DENABLE_SANITIZERS=ON` to CMake.
- `Ninja`: the sanitizer wrapper scripts and CI-friendly flows use Ninja for reliable instrumentation and faster builds; Visual Studio generator + MSVC may not support all sanitizers.

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
	```
	Builds all sample projects for Debug or Release. Output executables are in `bin/Debug` or `bin/Release`.

4. **Manual Build (Visual Studio)**
	Open `build/Playground.sln` in Visual Studio. Build using Debug/Release. Default startup project: `ExampleD3D12`.

# Batch Script Reference

The table below lists key batch scripts, their purpose, and copy-pastable usage examples (run from the repository root).

| Script | Purpose | Usage |
|--------|---------|-------|
| `CheckDependencies.bat` | Verify required tools are available on PATH (CMake, clang/clang++, msbuild, ninja). | `CheckDependencies.bat` |
| `CleanIntermediateFiles.bat` (root) | Remove `build/`, `bin/`, `obj/`, `.vs/` and other generated files. Interactive by default. | `CleanIntermediateFiles.bat`  or `CleanIntermediateFiles.bat CONTINUE` |
| `BuildSolution.bat` | Run dependency checks, optionally clean, and generate project files with CMake (into `build/`). | `BuildSolution.bat` |
| `BuildSamples.bat` (root, interactive) | User-friendly entrypoint: prompts for build configuration (Debug / Release / RelWithDebInfo) and delegates to the internal implementation. | `BuildSamples.bat` or `BuildSamples.bat Debug` |
| `tools\BuildSamplesDebug.bat` | Convenience wrapper to build samples with `Debug`. | `tools\BuildSamplesDebug.bat` |
| `tools\BuildSamplesRelease.bat` | Convenience wrapper to build samples with `Release`. | `tools\BuildSamplesRelease.bat` |
| `tools\BuildSamplesRelWithDebInfo.bat` | Convenience wrapper to build samples with `RelWithDebInfo` (optimized + debug symbols). | `tools\BuildSamplesRelWithDebInfo.bat` |
| `tools\internal\BuildSamplesImpl.bat` (internal) | Implementation that discovers sample projects and invokes MSBuild or the Ninja/clang flow. Intended for use by root/wrapper scripts. | (internal) — call via `BuildSamples.bat` |
| `tools\StartLogged.bat` (optional) | Run another script and capture stdout/stderr into `Saved\LogBuild.txt` with rotation (keeps up to 9 backups). | `tools\StartLogged.bat BuildSamples.bat Release` |

Common flows (copy-paste):

Interactive generate+build:
```powershell
BuildSolution.bat
BuildSamples.bat
```

Non-interactive generate + Release build:
```powershell
BuildSolution.bat
BuildSamples.bat Release
```

Notes:
- MSBuild outputs are placed under `bin\<Configuration>` (e.g. `bin\Debug`).
- `RelWithDebInfo` builds with optimizations enabled but retains full debug symbols — useful for sanitizers, profiling, and closer-to-release testing.
- Use `CONTINUE` to run cleaning scripts non-interactively (no pause on completion).
The root `BuildSamples.bat` delegates to the internal implementation at `tools\internal\BuildSamplesImpl.bat`. Output executables are placed under `bin\<Config>` (e.g. `bin\Debug`).

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
│   │   │   ├── D3D12/                # DirectX 12 Backend
│   │   │   │   ├── D3D12Rhi.*         # Device, queues, allocators, fences
│   │   │   │   ├── D3D12SwapChain.*   # Present/back buffers + RTVs
│   │   │   │   ├── D3D12DebugLayer.*  # GPU validation + InfoQueue
│   │   │   │   ├── Descriptors/       # Descriptor heaps, handles, allocators
│   │   │   │   ├── Pipeline/          # Root signatures, samplers, pipeline state
│   │   │   │   ├── Resources/         # Constant buffers, depth, upload buffers, frame resources
│   │   │   │   └── Shaders/           # DXC compilation (HLSL → DXIL)
│   │   │   │
│   │   │   └── Vulkan/               # Vulkan backend placeholder (future)
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
| **RHI** | D3D12 backend wrappers: device/queues, descriptors, pipeline state, swap chain |
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

## License
MIT License. See `LICENSE.txt` for details.