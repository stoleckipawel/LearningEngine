// ============================================================================
// PCH.h - SparkleUI Module
// ----------------------------------------------------------------------------
// Precompiled header for UI module.
//
// RULES:
// - Only STL headers (stable, rarely change)
// - NO cross-module includes (no Core/PCH.h)
// - NO imgui.h (changes frequently during UI development)
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
#include <cstdlib>
#include <memory>
#include <vector>
#include <string>
#include <functional>

// ============================================================================
// Engine Logging - Available everywhere via PCH
// ============================================================================
#include "Log.h"
