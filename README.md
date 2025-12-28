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
- **Clean RHI Abstraction** — Backend-agnostic design
- **Real-time Debug UI** — Integrated ImGui with GPU stats overlay  
- **Zero-friction Build** — Single-click CMake + MSBuild workflow  

</td>
<td width="50%">

### ⚡ Quick Start (30 seconds)

```batch
:: 1. Generate Visual Studio solution
BuildSolution.bat

:: 2. Build & run sample
BuildSamples.bat Release
```

</td>
</tr>
</table>

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              APPLICATION LAYER                              │
│                         samples/ExampleD3D12/main.cpp                       │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                               ENGINE PUBLIC API                             │
│                    App.h  ·  EngineConfig.h  ·  Log.h                       │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
        ┌─────────────┬───────────────┼───────────────┬─────────────┐
        ▼             ▼               ▼               ▼             ▼
   ┌─────────┐  ┌──────────┐   ┌───────────┐   ┌──────────┐   ┌─────────┐
   │  Core   │  │ Platform │   │  Renderer │   │ Resources│   │   UI    │
   │─────────│  │──────────│   │───────────│   │──────────│   │─────────│
   │ Timer   │  │ Window   │   │ Camera    │   │ Texture  │   │ ImGui   │
   │ Log     │  │ Input    │   │ Passes    │   │ Loader   │   │ Panels  │
   │ Memory  │  │ Events   │   │ Pipeline  │   │ Assets   │   │ Overlay │
   └─────────┘  └──────────┘   └───────────┘   └──────────┘   └─────────┘
        │             │               │               │             │
        └─────────────┴───────────────┼───────────────┴─────────────┘
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        RENDER HARDWARE INTERFACE (RHI)                      │
│─────────────────────────────────────────────────────────────────────────────│
│  ┌─────────────────────────────────┐    ┌─────────────────────────────────┐ │
│  │         D3D12 Backend           │    │      Vulkan Backend (Future)    │ │
│  │─────────────────────────────────│    │─────────────────────────────────│ │
│  │ • Device & Command Queues       │    │ •                               │ │
│  │ • Descriptor Heap Management    │    │ •                               │ │
│  │ • Pipeline State Objects        │    │ •                               │ │
│  │ • Root Signatures               │    │ •                               │ │
│  │ • Swap Chain (DXGI)             │    │ •                               │ │
│  │ • DXC Shader Compilation        │    │ •                               │ │
│  │ • GPU Debug Layer               │    │ •                               │ │
│  └─────────────────────────────────┘    └─────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Technical Highlights

<table>
<tr>
<td width="50%">

### DirectX 12 Implementation

| Feature | Implementation |
|---------|----------------|
| **Descriptor Handling** | Staged CPU→GPU descriptor copies |
| **Synchronization** | Fence-based CPU/GPU coordination |
| **Swap Chain** | Triple-buffered with DXGI flip model |
| **Debug Layer** | Full GPU validation + ID3D12InfoQueue |

</td>
<td width="50%">

### Shader Pipeline

| Feature | Implementation |
|---------|----------------|
| **Compiler** | DirectX Shader Compiler (DXC) |
| **Target** | Shader Model 6.0+ (DXIL) |
| **Language** | HLSL with modern features |

</td>
</tr>
<tr>
<td width="50%">

</td>
<td width="50%">

### Code Quality

- **C++20 Standard** — Concepts, ranges, designated initializers  
- **Precompiled Headers** — Fast incremental builds  
- **ClangFormat** — Consistent code style enforcement  
- **Modular Design** — Clear separation of concerns  
- **RAII Patterns** — Automatic resource cleanup  
- **Debug Utilities** — Comprehensive logging system  

</td>
</tr>
</table>

---

## 📁 Project Structure

```
Sparkle/
│
├── 📦 engine/                        # SparkleEngine static library
│   │
│   ├── include/                      #   PUBLIC API (exposed to consumers)
│   │   ├── App.h                     #   Application lifecycle interface
│   │   ├── EngineConfig.h            #   Build-time configuration
│   │   └── Log.h                     #   Logging facade
│   │
│   ├── src/                          # ๐Ÿ"' PRIVATE IMPLEMENTATION
│   │   │
│   │   ├── Core/                     #   Foundation layer
│   │   │   ├── Timer.cpp/h           #     High-precision timing
│   │   │   ├── Log.cpp               #     Logging implementation
│   │   │   ├── LinearAllocator.h     #     Fast frame allocator
│   │   │   └── PCH.h                 #     Precompiled header
│   │   │
│   │   ├── Platform/                 #   OS abstraction
│   │   │   └── Window.cpp/h          #     Win32 window management
│   │   │
│   │   ├── RHI/                      #   Render Hardware Interface
│   │   │   │
│   │   │   └── D3D12/                #   ◤ DirectX 12 Backend ◢
│   │   │       ├── D3D12Rhi.cpp/h              # Device, queues, fences
│   │   │       ├── D3D12SwapChain.cpp/h        # Present chain + RTVs
│   │   │       ├── D3D12DebugLayer.cpp/h       # GPU validation
│   │   │       │
│   │   │       ├── Descriptors/                # Descriptor management
│   │   │       │   ├── D3D12DescriptorHeap.*       # Heap wrapper
│   │   │       │   ├── D3D12DescriptorHandle.*     # CPU/GPU handles
│   │   │       │   └── D3D12DescriptorAllocator.*  # Dynamic allocation
│   │   │       │
│   │   │       ├── Pipeline/                   # PSO & root signatures
│   │   │       │   ├── D3D12PipelineState.*        # Graphics PSO
│   │   │       │   ├── D3D12RootSignature.*        # Parameter binding
│   │   │       │   └── D3D12Samplers.*             # Texture sampling
│   │   │       │
│   │   │       ├── Resources/                  # GPU resources
│   │   │       │   ├── D3D12ConstantBuffer.*       # Typed uploads
│   │   │       │   ├── D3D12DepthBuffer.*          # Depth/stencil
│   │   │       │   ├── D3D12UploadBuffer.*         # CPU→GPU staging
│   │   │       │   └── D3D12FrameResources.*       # Per-frame data
│   │   │       │
│   │   │       └── Shaders/                    # Shader compilation
│   │   │           └── DxcShaderCompiler.*         # HLSL → DXIL
│   │   │
│   │   ├── Renderer/                 #   High-level rendering
│   │   │   ├── Renderer.cpp/h        #     Render loop orchestration
│   │   │   └── Camera.cpp/h          #     View/projection matrices
│   │   │
│   │   ├── Resources/                #   Asset management
│   │   │   ├── Texture.cpp/h         #     Texture resource wrapper
│   │   │   └── TextureLoader.cpp/h   #     WIC-based loading
│   │   │
│   │   ├── Scene/                    #   Geometry & primitives
│   │   │   └── Primitives/           #     Procedural geometry
│   │   │
│   │   └── UI/                       #   Debug interface
│   │       ├── UI.cpp/h              #     ImGui integration
│   │       └── Panels/               #     Modular UI components
│   │
│   ├── assets/
│   │   └── shaders/                  # ⚡ HLSL shaders
│   │       ├── SimpleVS.hlsl         #     Vertex shader
│   │       ├── SimplePS.hlsl         #     Pixel shader
│   │       ├── ConstantBufferData.hlsli  # CB definitions (mirrors C++)
│   │       ├── Transform.hlsli       #     Matrix operations
│   │       └── Lighting.hlsli        #     Lighting calculations
│   │
│   └── third_party/
│       ├── d3dx12.h                  #   D3D12 helper library
│       └── imgui/                    #   Dear ImGui (docking branch)
│
├── 📂 samples/                       # Example applications
│   └── ExampleD3D12/
│       └── src/main.cpp              #   Minimal D3D12 sample
│
├── 📂 tools/                         # Build utilities
│   ├── BuildSamplesDebug.bat
│   ├── BuildSamplesRelease.bat
│   └── RunClangFormat.bat
│
├── 📄 CMakeLists.txt                 # Root CMake configuration
├── 📄 BuildSolution.bat              # Generate VS solution
├── 📄 BuildSamples.bat               # Build all samples
└── 📄 CheckDependencies.bat          # Verify toolchain
```

---

## 🚀 Getting Started

### Prerequisites

| Tool | Version | Required | Purpose |
|------|---------|:--------:|---------|
| **Visual Studio 2022** | 17.0+ | ✅ | C++ compiler & IDE |
| **Windows 10/11 SDK** | 10.0.19041+ | ✅ | DirectX 12 headers & libs |
| **CMake** | 3.20+ | ✅ | Build system generation |
| **Clang/LLVM** | 15.0+ | ⚪ | ClangCL toolset (optional) |

### Build Instructions

<table>
<tr>
<td width="50%">

#### Option A: Batch Scripts (Recommended)

```batch
:: Verify toolchain is installed
CheckDependencies.bat

:: Generate Visual Studio 2022 solution
BuildSolution.bat

:: Build all samples (choose configuration)
BuildSamples.bat Debug
BuildSamples.bat Release
BuildSamples.bat RelWithDebInfo
```

</td>
<td width="50%">

#### Option B: Visual Studio

```batch
:: Generate solution first
BuildSolution.bat
```

1. Open `build/Sparkle.sln`  
2. Set **ExampleD3D12** as startup project  
3. Select **Debug** or **Release**  
4. Press **F5** to build and run  

</td>
</tr>
</table>

#### Option C: CMake CLI

```bash
# Configure with Visual Studio 2022 generator
cmake -B build -G "Visual Studio 17 2022" -A x64 -T ClangCL

# Build Release configuration
cmake --build build --config Release

# Run sample
.\bin\Release\ExampleD3D12.exe
```

### Output Directories

| Directory | Contents |
|-----------|----------|
| `build/` | CMake cache, VS solution, project files |
| `bin/Debug/` | Debug executables + PDBs |
| `bin/Release/` | Optimized release executables |
| `bin/RelWithDebInfo/` | Optimized with debug symbols |

---

## 🛠️ Build Scripts Reference

| Script | Description |
|--------|-------------|
| `CheckDependencies.bat` | Verify CMake, MSBuild, Clang availability |
| `BuildSolution.bat` | Generate VS solution via CMake |
| `BuildSamples.bat [Config]` | Build samples (interactive or specify config) |
| `CleanIntermediateFiles.bat` | Remove all generated files |
| `tools/RunClangFormat.bat` | Format all source files |

---

## 📊 Module Reference

| Module | Layer | Responsibility |
|--------|-------|----------------|
| **Core** | Foundation | Timing, logging, memory utilities — zero graphics dependencies |
| **Platform** | Foundation | Win32 window creation, message pump, input handling |
| **RHI/D3D12** | Backend | Complete D3D12 abstraction: device, queues, heaps, PSOs, shaders |
| **Renderer** | High-Level | Camera matrices, render pass orchestration, frame management |
| **Resources** | High-Level | Texture loading (WIC), asset path resolution |
| **Scene** | High-Level | Procedural geometry generation (box, plane, polyhedra) |
| **UI** | High-Level | ImGui integration, debug overlays, parameter tweaking |

---

## 🧹 Troubleshooting

| Issue | Solution |
|-------|----------|
| **Missing SDK** | Install Windows 10/11 SDK via Visual Studio Installer |
| **CMake not found** | Add CMake to PATH or install via VS Installer |
| **Build errors after pull** | Run `CleanIntermediateFiles.bat` then rebuild |
| **D3D12 device creation fails** | Update GPU drivers; ensure D3D12 compatible hardware |
| **Shader compilation errors** | Check DXC is accessible; verify HLSL syntax |

---

## 📜 License

MIT License — See [LICENSE.txt](LICENSE.txt) for details.

---

<p align="center">
  <sub>Built with ❤️ for learning and experimentation</sub>
</p>