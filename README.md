<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue?style=for-the-badge&logo=cplusplus" alt="C++20"/>
  <img src="https://img.shields.io/badge/DirectX-12-green?style=for-the-badge&logo=microsoft" alt="DirectX 12"/>
  <img src="https://img.shields.io/badge/HLSL-SM%206.0%2B-purple?style=for-the-badge" alt="HLSL SM 6.0+"/>
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge" alt="MIT License"/>
</p>

<h1 align="center">Sparkle Engine</h1>

<p align="center">
  <strong>A modern DirectX 12 rendering engine built with clean architecture principles</strong>
</p>

---

## Key Features

| | Category | Highlights |
|:-:|----------|------------|
| :art: | **Graphics** | DirectX 12 with explicit resource management, descriptor heaps, triple-buffered swap chain |
| :movie_camera: | **Camera** | 3-tier architecture (GameCamera  RenderCamera  Frustum) with FPS controls and culling |
| :video_game: | **Input** | Layer-based event routing, RAII subscriptions, mouse capture, pollable state |
| :triangular_ruler: | **Geometry** | 15 procedural primitives including Platonic solids and subdivision surfaces |
| :straight_ruler: | **Depth** | Configurable Reversed-Z for superior precision in large scenes |
| :file_folder: | **Assets** | Marker-based discovery with compile-time hashed asset IDs |
| :desktop_computer: | **UI** | Integrated ImGui with debug panels and stats overlay |

---

## Architecture

`
APPLICATION       projects/<YourProject>/main.cpp
       
ENGINE API        App.h    EngineConfig.h    Log.h
       
SYSTEMS           Core    Platform    Renderer    Scene    UI
       
RHI               D3D12 Backend (Device, Heaps, PSO, Shaders, Resources)
`

---

## Quick Start

| Step | Command | Description |
|:----:|---------|-------------|
| 1 | `BuildSolution.bat` | Generate Visual Studio solution |
| 2 | `BuildProjects.bat Release` | Build all projects |
| 3 | `CreateNewProject.bat MyGame` | Create new project from template |

**Prerequisites:** Visual Studio 2022 (17.0+)  Windows SDK (10.0.19041+)  CMake (3.20+)

---

## Technical Highlights

<table>
<tr>
<td width="50%" valign="top">

### DirectX 12

| Feature | |
|---------|:-:|
| Managed descriptor heap allocation | :white_check_mark: |
| Root signature with CBV/SRV/UAV | :white_check_mark: |
| Fence-based CPU/GPU sync | :white_check_mark: |
| Frame resources + linear allocator | :white_check_mark: |
| GPU validation & debug layer | :white_check_mark: |

</td>
<td width="50%" valign="top">

### Engine Systems

| Feature | |
|---------|:-:|
| 3-tier camera with dirty flags | :white_check_mark: |
| Event-driven layered input | :white_check_mark: |
| DXC shader compilation (SM 6.0+) | :white_check_mark: |
| WIC texture loading | :white_check_mark: |
| PBR-ready shader library | :white_check_mark: |

</td>
</tr>
</table>

---

## Design Patterns & Decisions

<table>
<tr>
<td width="50%" valign="top">

### Patterns Used

| Pattern | Application |
|---------|-------------|
| **RAII** | D3D12 resource wrappers |
| **NVI** | App lifecycle hooks |
| **Observer** | Input & depth events |
| **Factory** | Mesh generation |
| **Strategy** | Input backends |

</td>
<td width="50%" valign="top">

### Key Decisions

| Decision | Rationale |
|----------|-----------|
| **3-Tier Camera** | Separates game, render, cull |
| **Reversed-Z** | Better depth precision |
| **Frame Resources** | No CPU/GPU stalls |
| **Compile-Time Hash** | Zero-cost asset IDs |

</td>
</tr>
</table>

---

## Project Structure

`
engine/
 include/            Public API (App, Config, Log)
 src/
    Core/           Timer, Math, Events
    Platform/       Window, InputSystem
    Renderer/       RenderCamera, DepthConvention, Textures
    RHI/D3D12/      Device, Heaps, Pipeline, Shaders, Resources
    Scene/          MeshFactory, 15 Primitives
    UI/             ImGui Panels
 Assets/Shaders/     HLSL (BRDF, Lighting, Passes)

projects/
 HelloWorld/         Minimal example
 SampleScenes/       Primitive showcase
 TemplateProject/    New project template
`

---

<p align="center">
  <strong>MIT License</strong>  See <a href="LICENSE.txt">LICENSE.txt</a>
</p>

<p align="center">
  <sub>Built with modern C++20 and DirectX 12</sub>
</p>
