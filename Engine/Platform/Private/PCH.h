// ============================================================================
// PCH.h - SparklePlatform Module
// ----------------------------------------------------------------------------
// Precompiled header for Platform module.
//
// RULES:
// - Only STL and Windows headers (stable, rarely change)
// - NO cross-module includes
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
#include <string>
#include <string_view>
#include <memory>
#include <functional>
#include <algorithm>

// ============================================================================
// Windows - Core platform dependency
// ============================================================================
#include <Windows.h>

// ============================================================================
// Engine Logging - Available everywhere via PCH
// ============================================================================
#include "Diagnostics/Log.h"
