# LearningEngine

## Overview

LearningEngine is a DirectX 12 playground engine for experimentation and learning. It features a modular engine and a sample project (`DX12Toy`).

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
	Open `build/Playground.sln` in Visual Studio. Build using Debug/Release. Default startup project: `DX12Toy`.

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

- `engine/` — Core engine code, headers, assets
- `samples/` — Sample projects using the engine
- `build/` — Generated build files and solution
- `bin/` — Compiled binaries

## Troubleshooting

- Missing dependencies: Ensure Windows 10 SDK and DirectX 12 libraries are installed.
  ```
  CheckDependencies.bat
  ```
  Check for CMake, Clang, MSBuild 

- Build errors or workspace reset:
  ```
  CleanSolution.bat
  ```
  Removes all intermediate, build, and temporary files. After cleaning, run `BuildSolution.bat` to restore solution files.

## License

MIT License. See `LICENSE.txt` for details.