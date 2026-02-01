// ============================================================================
// UIConfig.h
// ----------------------------------------------------------------------------
// UI module configuration constants and compile-time toggles.
//
// DESIGN:
//   - Header-only with minimal dependencies
//   - Compile-time macros for UI features
//
// NOTES:
//   - This keeps UI self-contained without coupling to Core
// ============================================================================

#pragma once

// ============================================================================
// Compile-Time Feature Toggles
// ============================================================================

/// Set to 0 to completely exclude GUI from builds.
#define USE_GUI 1

/// Uncomment to enable ImGui demo window for development/testing.
// #define USE_IMGUI_DEMO_WINDOW 1
