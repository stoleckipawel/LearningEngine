
#pragma once

// Precompiled header for Windows/DirectX engine

// Windows macros
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

// Standard library
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <string>
#include <cstring>
#include <string_view>
#include <fstream>
#include <iostream>

// Windows
#include <Windows.h>
#include "wincodec.h"

// DirectX
#include "d3d12.h"
#include "d3dx12.h"
#include <DirectXMath.h>
#include "dxgi1_6.h"
#include "D3Dcompiler.h"
#include "ComPointer.h"

// DirectX debug
#ifdef _DEBUG
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#endif

using namespace DirectX;

// Error handling utilities
#include "Error.h"
