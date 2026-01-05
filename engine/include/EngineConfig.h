// ============================================================================
// EngineConfig.h
// ----------------------------------------------------------------------------
// Engine-wide configuration constants and compile-time toggles.
//
// DESIGN:
//   - Header-only with minimal dependencies for cheap inclusion everywhere
//   - Compile-time macros for feature toggles (GUI, validation, etc.)
//   - Runtime-configurable settings in EngineSettings namespace
//
// NOTES:
//   - Modify EngineSettings values at runtime for app-specific behavior
//   - FramesInFlight affects resource allocation and latency
// ============================================================================

#pragma once

#include <string>
#include <dxgi1_6.h>

// ============================================================================
// Compile-Time Feature Toggles
// ============================================================================

/// Set to 0 to completely exclude GUI from builds.
#define USE_GUI 1
// #define USE_IMGUI_DEMO_WINDOW 1

/// Shader compilation flags (enabled by default in debug builds).
#if defined(_DEBUG)
	#define ENGINE_SHADERS_OPTIMIZED 1  ///< Enable shader optimizations
	#define ENGINE_SHADERS_DEBUG 1      ///< Include shader debug info
#endif

/// GPU validation layers (D3D12/DXGI SDK layers).
#if defined(_DEBUG)
	#define ENGINE_GPU_VALIDATION 1
#endif

/// Report live D3D/DXGI objects at shutdown for leak detection.
#if defined(_DEBUG)
	#define ENGINE_REPORT_LIVE_OBJECTS 1
#endif

// ============================================================================
// Runtime Configuration
// ============================================================================

namespace EngineSettings
{
	// ------------------------------------------------------------------------
	// Rendering
	// ------------------------------------------------------------------------

	/// Number of frames that can be processed simultaneously.
	/// Higher values reduce CPU-GPU sync but increase latency and memory.
	inline constexpr unsigned FramesInFlight = 2u;

	/// Back buffer pixel format.
	inline constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	/// Enable vertical sync. False allows uncapped presents or tearing.
	inline bool VSync = true;

	// ------------------------------------------------------------------------
	// Window
	// ------------------------------------------------------------------------

	/// Start in fullscreen mode when true.
	inline bool StartFullscreen = false;

	// ------------------------------------------------------------------------
	// Hardware
	// ------------------------------------------------------------------------

	/// Prefer high-performance GPU when enumerating adapters.
	inline bool PreferHighPerformanceAdapter = true;

	// ------------------------------------------------------------------------
	// Shaders
	// ------------------------------------------------------------------------

	/// Target shader model version (e.g., 6.0 for SM 6.0).
	inline constexpr int ShaderModelMajor = 6;
	inline constexpr int ShaderModelMinor = 0;

}  // namespace EngineSettings
