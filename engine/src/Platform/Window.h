// =============================================================================
// Window.h — Platform Window Management
// =============================================================================
//
// Manages the main application window, message handling, and display state.
// Encapsulates Win32 window creation and provides a clean interface for
// querying window dimensions and fullscreen state.
//
// USAGE:
//   GWindow.Initialize();                    // Create and show window
//   while (!GWindow.ShouldClose()) {
//       GWindow.PollEvents();                // Process Win32 messages
//       // ... render frame ...
//   }
//   GWindow.Shutdown();                      // Destroy window
//
// NOTES:
//   - Singleton accessed via GWindow global
//   - Initialize() must be called before any D3D12 initialization
//   - Handles WM_SIZE for resize events, WM_CLOSE for shutdown
//   - Fullscreen uses borderless windowed mode (not exclusive)
//
// =============================================================================

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <string_view>

// =============================================================================
// Window
// =============================================================================

class Window final
{
  public:
	[[nodiscard]] static Window& Get() noexcept;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	// =========================================================================
	// Lifecycle
	// =========================================================================

	// Creates the application window using app-provided parameters and EngineSettings.
	void Initialize(std::string_view windowTitle);

	// Destroys the window and unregisters the window class.
	void Shutdown();

	// =========================================================================
	// Message Pump
	// =========================================================================

	// Processes pending Win32 messages. Call once per frame.
	void PollEvents() noexcept;

	// =========================================================================
	// Accessors
	// =========================================================================

	[[nodiscard]] HWND GetHWND() const noexcept { return m_windowHWND; }

	[[nodiscard]] RECT GetRect() const noexcept;
	[[nodiscard]] UINT GetWidth() const noexcept;
	[[nodiscard]] UINT GetHeight() const noexcept;

	// Returns viewport dimensions as float2 (for shader constants).
	[[nodiscard]] DirectX::XMFLOAT2 GetViewportSize() const noexcept;

	// Returns 1/viewport dimensions (for pixel-to-UV conversion).
	[[nodiscard]] DirectX::XMFLOAT2 GetViewportSizeInv() const noexcept;

	// =========================================================================
	// State
	// =========================================================================

	// Toggles fullscreen mode (borderless windowed).
	void SetFullScreen(bool bSetFullScreen);

	[[nodiscard]] bool ShouldClose() const noexcept { return m_bShouldClose; }
	[[nodiscard]] bool IsFullScreen() const noexcept { return m_bIsFullScreen; }

  private:
	Window() = default;
	~Window() = default;

	// -------------------------------------------------------------------------
	// Win32 Internals
	// -------------------------------------------------------------------------

	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND m_windowHWND = nullptr;
	ATOM m_wndClass = 0;
	bool m_bShouldClose = false;
	bool m_bIsFullScreen = false;
};

inline Window& GWindow = Window::Get();