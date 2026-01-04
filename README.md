<p align="center">
  <h1 align="center">✨ Sparkle Engine</h1>
  <p align="center">
    <strong>A modern DirectX 12 rendering engine built with clean architecture principles</strong>
  </p>
  <p align="center">
    <code>C++20</code> · <code>DirectX 12</code> · <code>HLSL</code> · <code>DXC Shader Model 6.0+</code>
  </p>
</p>

---

<table>
<tr>
<td width="50%">

### 🎯 At a Glance

- **Modern D3D12** — Direct GPU control with explicit resource management  
- **DXC Shader Compiler** — HLSL to DXIL with Shader Model 6.0+ features  
- **Asset System** — Marker-based path discovery with compile-time asset IDs  
- **Project Generator** — One-click project scaffolding like professional engines  
- **Real-time Debug UI** — Integrated ImGui with renderer panels  
- **Zero-friction Build** — Single-click CMake + MSBuild workflow  

</td>
<td width="50%">

### ⚡ Quick Start (30 seconds)

```batch
:: 1. Generate Visual Studio solution
BuildSolution.bat

:: 2. Build & run projects
BuildProjects.bat Release

:: 3. Create your own project!
CreateNewProject.bat MyGame
```

</td>
</tr>
</table>

---

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              APPLICATION LAYER                              │
│                        projects/<YourProject>/src/main.cpp                  │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                               ENGINE PUBLIC API                             │
│                      App.h  ·  EngineConfig.h  ·  Log.h                     │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
        ┌─────────────┬───────────────┼───────────────┬─────────────┐
        ▼             ▼               ▼               ▼             ▼
   ┌─────────┐  ┌──────────┐   ┌───────────┐   ┌──────────┐   ┌─────────┐
   │  Core   │  │ Platform │   │  Renderer │   │  Assets  │   │   UI    │
   │─────────│  │──────────│   │───────────│   │──────────│   │─────────│
   │ Timer   │  │ Window   │   │ Camera    │   │ AssetId  │   │ ImGui   │
   │ Log     │  │ Input    │   │ Depth     │   │ System   │   │ Panels  │
   │ Memory  │  │ Events   │   │ Pipeline  │   │ Types    │   │ Overlay │
   │ Hash    │  │          │   │           │   │          │   │         │
   └─────────┘  └──────────┘   └───────────┘   └──────────┘   └─────────┘
        │             │               │               │             │
        └─────────────┴───────────────┼───────────────┴─────────────┘
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        RENDER HARDWARE INTERFACE (RHI)                      │
│─────────────────────────────────────────────────────────────────────────────│
│                           DirectX 12 Backend                                │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │
│  │   Device &   │  │ Descriptors  │  │   Pipeline   │  │   Shaders    │    │
│  │   Queues     │  │   & Heaps    │  │   States     │  │   (DXC)      │    │
│  └──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │
│  │  Swap Chain  │  │  Resources   │  │   Samplers   │  │ Debug Layer  │    │
│  │   (DXGI)     │  │  & Buffers   │  │   Library    │  │ & Validation │    │
│  └──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘    │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🆕 Asset System

The engine features a **marker-based asset discovery system**.

### 📍 Marker Hierarchy

| Marker File | Location | Purpose |
|-------------|----------|---------|
| `.sparkle` | Repository root | Workspace identification |
| `.sparkle-engine` | `engine/` | Engine root discovery |
| `.sparkle-project` | `projects/<Name>/` | Project root discovery |

### 🏷️ Compile-Time Asset IDs

```cpp
// Zero runtime cost - hash computed at compile time
constexpr AssetId diffuseId = "textures/brick_diffuse.png"_asset;

// Use as map keys for O(1) lookups
std::unordered_map<AssetId, TextureHandle> textureCache;
textureCache[diffuseId] = LoadTexture(diffuseId);
```

### 📁 Asset Types

| Type | Directory |
|------|-----------|
| `Shader` | `shaders/` |
| `ShaderSymbols` | `shaders/ShaderSymbols/` |
| `Texture` | `textures/` |
| `Mesh` | `meshes/` |
| `Material` | `materials/` |
| `Scene` | `scenes/` |
| `Audio` | `audio/` |
| `Font` | `fonts/` |

---

## 🎮 Project Generation

Create new projects instantly with a single command — no manual setup required!

```batch
:: Interactive mode
CreateNewProject.bat

:: Or specify the name directly
CreateNewProject.bat MyAwesomeGame
```

### What Gets Created

```
projects/MyAwesomeGame/
├── .sparkle-project          # Project marker (auto-discovered by CMake)
├── CMakeLists.txt            # Pre-configured build setup
├── src/
│   └── main.cpp              # App subclass with lifecycle hooks
└── assets/
    ├── audio/
    ├── fonts/
    ├── materials/
    ├── meshes/
    ├── scenes/
    ├── shaders/
    └── textures/
```

### 🔄 How It Works

1. **Template Copy** — Copies `projects/TemplateProject/` to your new project
2. **Name Substitution** — Replaces `__PROJECT_NAME__` placeholders
3. **Marker Creation** — Creates `.sparkle-project` for CMake discovery
4. **Auto-Rebuild** — Optionally regenerates VS solution

---

## 🔧 Technical Highlights

<table>
<tr>
<td width="50%">

### 🖥️ DirectX 12 Implementation

| Feature | Implementation |
|---------|----------------|
| **Descriptor Heaps** | Managed allocation with CPU→GPU staging |
| **Synchronization** | Fence-based CPU/GPU coordination |
| **Swap Chain** | Triple-buffered DXGI flip model |
| **Debug Layer** | Full GPU validation + InfoQueue |
| **Constant Buffers** | Type-safe upload buffer management |
| **Depth/Stencil** | Configurable depth conventions |

</td>
<td width="50%">

### ⚡ Shader Pipeline

| Feature | Implementation |
|---------|----------------|
| **Compiler** | DirectX Shader Compiler (DXC) |
| **Target** | Shader Model 6.0+ (DXIL) |
| **Debug Symbols** | Full PDB support for debugging |
| **Includes** | BRDF, Lighting, Material libraries |
| **Passes** | GBuffer, Forward, Shadow, Debug |

</td>
</tr>
<tr>
<td width="50%">

### 🎨 Rendering Features

| Feature | Status |
|---------|--------|
| **Camera System** | ✅ View/Projection matrices |
| **Depth Conventions** | ✅ Configurable near/far planes |
| **Texture Loading** | ✅ WIC-based with format support |
| **Sampler Library** | ✅ Pre-built sampler states |
| **Mesh Factory** | ✅ Procedural geometry |
| **Scene Graph** | ✅ Basic scene management |

</td>
<td width="50%">

### 🛡️ Code Quality

| Practice | Implementation |
|----------|----------------|
| **C++20 Standard** | Concepts, ranges, constexpr |
| **Precompiled Headers** | Fast incremental builds |
| **ClangFormat** | Enforced code style |
| **RAII Patterns** | Automatic resource cleanup |
| **NVI Pattern** | Clean App lifecycle hooks |
| **Compile-Time Hashing** | Zero-cost asset IDs |

</td>
</tr>
</table>

---

## 📁 Project Structure

```
Sparkle/
│
├── 🔧 Build Scripts
│   ├── BuildSolution.bat         # Generate VS solution
│   ├── BuildProjects.bat         # Build all projects
│   ├── CreateNewProject.bat      # 🆕 Project generator
│   ├── CheckDependencies.bat     # Verify toolchain
│   └── CleanIntermediateFiles.bat
│
├── 📦 engine/                    # SparkleEngine static library
│   │
│   ├── include/                  # 🌐 PUBLIC API
│   │   ├── App.h                 #   Application lifecycle (NVI pattern)
│   │   ├── EngineConfig.h        #   Build configuration
│   │   └── Log.h                 #   Logging facade
│   │
│   ├── src/                      # 🔒 PRIVATE IMPLEMENTATION
│   │   │
│   │   ├── Assets/               # 🆕 Asset management system
│   │   │   ├── AssetSystem.*     #   Marker-based path discovery
│   │   │   ├── AssetId.h         #   Compile-time asset hashing
│   │   │   ├── AssetTypes.h      #   Asset classification enum
│   │   │   └── AssetSource.h     #   Engine vs Project assets
│   │   │
│   │   ├── Core/                 # Foundation utilities
│   │   │   ├── Time/Timer.*      #   High-precision timing
│   │   │   ├── Memory/           #   Linear allocator
│   │   │   ├── Hash/HashUtils.h  #   FNV-1a compile-time hashing
│   │   │   ├── Strings/          #   String utilities
│   │   │   ├── Diagnostics/      #   Debug utilities & logging
│   │   │   ├── FileSystemUtils.* #   Path discovery & normalization
│   │   │   └── PCH.h             #   Precompiled header
│   │   │
│   │   ├── Platform/             # OS abstraction
│   │   │   └── Window.*          #   Win32 window management
│   │   │
│   │   ├── Renderer/             # High-level rendering
│   │   │   ├── Renderer.*        #   Render loop orchestration
│   │   │   ├── Camera.*          #   View/projection matrices
│   │   │   └── DepthConvention.* #   Depth buffer configuration
│   │   │
│   │   ├── RHI/D3D12/            # ◤ DirectX 12 Backend ◢
│   │   │   ├── D3D12Rhi.*        #   Device, queues, fences
│   │   │   ├── D3D12SwapChain.*  #   Present chain + RTVs
│   │   │   ├── D3D12DebugLayer.* #   GPU validation
│   │   │   │
│   │   │   ├── Descriptors/      #   Heap & handle management
│   │   │   ├── Pipeline/         #   PSO & root signatures
│   │   │   ├── Resources/        #   Buffers & frame resources
│   │   │   ├── Samplers/         #   Sampler state library
│   │   │   └── Shaders/          #   DXC compiler integration
│   │   │
│   │   ├── Resources/            # Asset loading
│   │   │   ├── Texture.*         #   Texture resource wrapper
│   │   │   └── TextureLoader.*   #   WIC-based image loading
│   │   │
│   │   ├── Scene/                # Scene management
│   │   │   ├── Scene.*           #   Scene container
│   │   │   ├── Mesh.*            #   Mesh data structures
│   │   │   ├── MeshFactory.*     #   Procedural generation
│   │   │   └── Primitives/       #   Basic & polyhedra shapes
│   │   │
│   │   └── UI/                   # Debug interface
│   │       ├── UI.*              #   ImGui integration
│   │       ├── Panels/           #   Renderer debug panel
│   │       ├── Sections/         #   UI section components
│   │       └── Framework/        #   UI utilities
│   │
│   ├── assets/shaders/           # ⚡ HLSL Shader Library
│   │   ├── Common/               #   Shared utilities
│   │   ├── BRDF/                 #   PBR lighting models
│   │   ├── Lighting/             #   Light evaluation
│   │   ├── Material/             #   Material sampling
│   │   ├── Geometry/             #   Vertex processing
│   │   ├── Passes/               #   Render passes
│   │   │   ├── GBuffer/          #     Deferred geometry
│   │   │   ├── Forward/          #     Forward rendering
│   │   │   ├── Shadow/           #     Shadow mapping
│   │   │   ├── DeferredLighting/ #     Light accumulation
│   │   │   └── Debug/            #     Visualization
│   │   └── Raytracing/           #   RT shader stubs
│   │
│   └── third_party/              # External dependencies
│       ├── d3dx12.h              #   D3D12 helper library
│       └── imgui/                #   Dear ImGui
│
├── 📂 projects/                  # 🆕 Game projects (auto-discovered)
│   ├── TemplateProject/          #   Project template (not built)
│   ├── HelloWorld/               #   Example: minimal app
│   └── Sponza/                   #   Example: Sponza scene
│
└── 📂 tools/                     # Build utilities
    ├── BuildProjectsDebug.bat
    ├── BuildProjectsRelease.bat
    └── RunClangFormat.bat
```

---

## 🚀 Getting Started

### Prerequisites

| Tool | Version | Required | Purpose |
|------|---------|:--------:|---------|
| **Visual Studio 2022** | 17.0+ | ✅ | C++20 compiler & IDE |
| **Windows SDK** | 10.0.19041+ | ✅ | DirectX 12 headers & libs |
| **CMake** | 3.20+ | ✅ | Build system generation |
| **Clang/LLVM** | 15.0+ | ⚪ | ClangCL toolset (optional) |

### 🏃 Build & Run

<table>
<tr>
<td width="50%">

#### Option A: Batch Scripts (Recommended)

```batch
:: 1. Verify toolchain
CheckDependencies.bat

:: 2. Generate VS solution
BuildSolution.bat

:: 3. Build projects
BuildProjects.bat Debug
```

</td>
<td width="50%">

#### Option B: Visual Studio

```batch
:: Generate solution first
BuildSolution.bat
```

1. Open `build/Sparkle.sln`  
2. Right-click project → **Set as Startup**  
3. Press **F5** to build and run  

</td>
</tr>
</table>

#### Option C: CMake CLI

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
.\bin\Release\HelloWorld.exe
```

### 📂 Output Directories

| Directory | Contents |
|-----------|----------|
| `build/` | CMake cache, VS solution, project files |
| `bin/Debug/` | Debug executables + PDBs |
| `bin/Release/` | Optimized release builds |
| `bin/RelWithDebInfo/` | Optimized with debug symbols |

---

## 📊 Module Reference

| Module | Layer | Responsibility |
|--------|-------|----------------|
| **Assets** | Foundation | Marker-based discovery, compile-time asset IDs, path resolution |
| **Core** | Foundation | Timing, logging, hashing, memory utilities |
| **Platform** | Foundation | Win32 window creation, message pump |
| **RHI/D3D12** | Backend | Complete D3D12 abstraction: device, heaps, PSOs, shaders |
| **Renderer** | High-Level | Camera, depth handling, render orchestration |
| **Resources** | High-Level | Texture loading (WIC), asset management |
| **Scene** | High-Level | Mesh factory, procedural geometry, scene graph |
| **UI** | High-Level | ImGui integration, debug panels |

---

## 🛠️ Build Scripts Reference

| Script | Description |
|--------|-------------|
| `BuildSolution.bat` | Generate VS solution via CMake |
| `BuildProjects.bat` | Build all discovered projects |
| `CreateNewProject.bat` | 🆕 Create new project from template |
| `CheckDependencies.bat` | Verify CMake, MSBuild availability |
| `CleanIntermediateFiles.bat` | Remove all generated files |
| `tools/RunClangFormat.bat` | Format all source files |

---

## 🧹 Troubleshooting

| Issue | Solution |
|-------|----------|
| **Project not discovered** | Ensure `.sparkle-project` marker exists in project root |
| **Missing SDK** | Install Windows SDK via Visual Studio Installer |
| **CMake not found** | Add CMake to PATH or install via VS Installer |
| **Build errors after pull** | Run `CleanIntermediateFiles.bat` then rebuild |
| **D3D12 device fails** | Update GPU drivers; verify D3D12 hardware support |
| **Shader errors** | Check DXC availability; verify HLSL syntax |

---

## 📜 License

MIT License — See [LICENSE.txt](LICENSE.txt) for details.

---

<p align="center">
  <sub>Built with ❤️ using modern C++ and DirectX 12</sub>
</p>
