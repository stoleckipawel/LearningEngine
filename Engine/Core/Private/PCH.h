// ============================================================================
// PCH.h - SparkleCore Module
// Precompiled header for Core module.
// Core module should NOT include platform-specific or RHI headers.
// ============================================================================

#pragma once

// Windows macros
#define NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#endif

// Standard library
#include <chrono>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <string>
#include <cstring>
#include <string_view>
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <functional>
#include <cstdint>

// Windows (basic)
#include <Windows.h>

// Core module headers
#include "Diagnostics/Log.h"
#include "EngineConfig.h"
