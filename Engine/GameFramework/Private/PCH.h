// ============================================================================
// PCH.h - SparkleGameFramework Module
// ----------------------------------------------------------------------------
// Precompiled header for GameFramework module.
//
// RULES:
// - Only STL headers and DirectXMath (stable, rarely change)
// - NO cross-module includes (no Core/PCH.h)
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
#include <utility>

// ============================================================================
// DirectX Math - Used extensively in scene/mesh/transform code
// ============================================================================
#include <DirectXMath.h>

// ============================================================================
// Engine Logging - Available everywhere via PCH
// ============================================================================
#include "Log.h"
