// ============================================================================
// UI.h
// ----------------------------------------------------------------------------
// ImGui integration layer for Win32 and D3D12 backends.
//
// USAGE:
//   GUI.Initialize();      // During startup
//   // In message loop:
//   GUI.OnWindowMessage(hwnd, msg, wParam, lParam);
//   // Each frame:
//   GUI.Update();          // Build draw lists
//   GUI.Render();          // Submit to GPU
//   GUI.Shutdown();        // During teardown
//
// DESIGN:
//   - Wraps ImGui context creation/destruction
//   - Manages Win32 input forwarding and DX12 rendering
//   - Owns UI panels (RendererPanel for settings)
//
// THREADING:
//   All ImGui calls must be made from the main thread where the
//   device/command list are used. This class is not thread-safe.
//
// NOTES:
//   - Singleton accessed via GUI global reference
//   - DPI scaling is applied automatically during initialization
// ============================================================================

#pragma once

#include <Windows.h>

#include <memory>

#include "Sections/ViewMode.h"

class RendererPanel;
class UIRendererSection;
class StatsOverlay;

class UI final
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Returns the singleton UI instance.
	[[nodiscard]] static UI& Get() noexcept;

	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;
	UI(UI&&) = delete;
	UI& operator=(UI&&) = delete;

	/// Creates ImGui context and initializes Win32/DX12 backends.
	/// May log/abort on failure via engine error helpers.
	void Initialize();

	/// Shuts down backends and destroys ImGui context.
	void Shutdown() noexcept;

	// ========================================================================
	// Message Handling
	// ========================================================================

	/// Forwards Win32 messages to ImGui for input processing.
	/// @return True if ImGui consumed the message and app should skip it.
	bool OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	// ========================================================================
	// Frame Operations
	// ========================================================================

	/// Updates UI state and builds draw lists for the current frame.
	void Update();

	/// Submits ImGui draw data to the current DX12 command list.
	void Render() noexcept;

	// ========================================================================
	// Accessors
	// ========================================================================

	/// Returns the current view mode selected in the UI.
	[[nodiscard]] ViewMode::Type GetViewMode() noexcept;

  private:
	UI() = default;
	~UI() noexcept;

	// ------------------------------------------------------------------------
	// Frame Building
	// ------------------------------------------------------------------------

	/// Begins an ImGui frame. Updates delta time and display size.
	void NewFrame();

	/// Builds UI content (panels, overlays) and finalizes draw data.
	void Build();

	// ------------------------------------------------------------------------
	// Initialization Helpers
	// ------------------------------------------------------------------------

	/// Configures ImGui font and style for system DPI.
	void SetupDPIScaling() noexcept;

	// ------------------------------------------------------------------------
	// Owned Panels
	// ------------------------------------------------------------------------

	std::unique_ptr<RendererPanel> m_rendererPanel;  ///< Settings panel for renderer options
};

/// Global singleton reference for convenient access.
inline UI& GUI = UI::Get();
