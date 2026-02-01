// ============================================================================
// PCH.h - SparkleRenderer Module
// ----------------------------------------------------------------------------
// Precompiled header for Renderer module.
//
// RULES:
// - Only STL headers and DirectXMath (stable, rarely change)
// - NO cross-module includes (no Core/PCH.h, no RHI headers)
// - Headers must be used in 50%+ of this module's .cpp files
// ============================================================================

#pragma once

// ============================================================================
// Windows Configuration
// ============================================================================
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

// ============================================================================
// C++ Standard Library
// ============================================================================
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

// ============================================================================
// DirectX Math - Used extensively in rendering code
// ============================================================================
#include <DirectXMath.h>

// ============================================================================
// Engine Logging - Available everywhere via PCH
// ============================================================================
#include "Log.h"
