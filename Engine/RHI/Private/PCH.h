// ============================================================================
// PCH.h - SparkleRHI Module
// ----------------------------------------------------------------------------
// Precompiled header for RHI module.
//
// RULES:
// - Only STL, Windows, and DirectX headers (stable, rarely change)
// - RHIConfig.h for compile-time feature toggles
// - Headers must be used in 50%+ of this module's .cpp files
// ============================================================================

#pragma once

// ============================================================================
// RHI Configuration - Required for ENGINE_GPU_VALIDATION macros
// ============================================================================
#include "RHIConfig.h"

// ============================================================================
// Windows Configuration
// ============================================================================
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

// ============================================================================
// C++ Standard Library - Commonly used across RHI
// ============================================================================
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <array>
#include <algorithm>

// ============================================================================
// Windows - Required for D3D12
// ============================================================================
#include <Windows.h>
#include <wrl/client.h>

// ============================================================================
// Engine Logging - Available everywhere via PCH
// ============================================================================
#include "Log.h"

// ============================================================================
// DirectX 12 - Core RHI dependency (used in 100% of .cpp files)
// ============================================================================
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include <d3dx12.h>

// ============================================================================
// DirectX Debug (conditional)
// ============================================================================
#ifdef ENGINE_GPU_VALIDATION
	#include <d3d12sdklayers.h>
	#include <dxgidebug.h>
#endif

// ============================================================================
// Convenience Aliases
// ============================================================================
using Microsoft::WRL::ComPtr;
