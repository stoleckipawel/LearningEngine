# LearningEngine

## Overview

LearningEngine is a DirectX 12 playground engine designed for experimentation and learning. It includes a modular engine and sample project (`DX12Toy`)


## Getting Started

### Regenerate Visual Studio Solution

To set up the Visual Studio solution and project files, run:

```
RegenerateSolution.bat
```

This script will:
- Clean old build, bin, obj, and `.vs` folders.
- Create a fresh `build` directory.
- Generate Visual Studio solution files using CMake.

**Note:** Ensure CMake is installed and available in your system `PATH`.

## Building and Running

After generating the solution, open `build/Playground.sln` in Visual Studio. Build the solution using your preferred configuration (Debug/Release). The default startup project is `DX12Toy`.

## Dependencies

- **CMake** (minimum version 3.20)
- **Visual Studio 2022** (or compatible version)
- **DirectX 12 SDK / Windows 10 SDK** (required for DX12 development)
- **Windows platform** (tested on Windows)

The engine uses DirectX 12 libraries. Make sure your system has the required SDKs installed.

## Directory Structure

- `engine/` — Core engine code, headers, assets
- `samples/DX12Toy/` — Sample project using the engine
- `build/` — Generated build files and solution
- `bin/` — Compiled binaries


## Troubleshooting

- If you encounter missing dependencies, ensure the Windows 10 SDK and DirectX 12 libraries are installed.
- If you experience build errors or want to reset the workspace, run:

	```
	CleanSolution.bat
	```
	This will remove all intermediate, build, and temporary files, allowing you to start fresh. After cleaning, you may need to run `RegenerateSolution.bat` again to restore the solution files.

## License

MIT License. See `LICENSE.txt` for details.