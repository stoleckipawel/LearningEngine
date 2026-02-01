// ============================================================================
// PlatformConfig.h
// ----------------------------------------------------------------------------
// Platform module configuration constants.
//
// DESIGN:
//   - Header-only with minimal dependencies
//   - Runtime-configurable settings in PlatformSettings namespace
//
// NOTES:
//   - This keeps Platform self-contained without coupling to Core
// ============================================================================

#pragma once

// ============================================================================
// Runtime Configuration
// ============================================================================

namespace PlatformSettings
{
	// ------------------------------------------------------------------------
	// Window
	// ------------------------------------------------------------------------

	/// Start in fullscreen mode when true.
	inline bool StartFullscreen = false;

}  // namespace PlatformSettings
