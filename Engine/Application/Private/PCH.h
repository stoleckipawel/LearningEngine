// ============================================================================
// PCH.h - SparkleApplication Module
// ----------------------------------------------------------------------------
// Precompiled header for Application module.
//
// RULES:
// - Only STL headers (stable, rarely change)
// - NO cross-module includes
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
#include <string>
#include <utility>

// ============================================================================
// Engine Logging - Available everywhere via PCH
// ============================================================================
#include "Log.h"
