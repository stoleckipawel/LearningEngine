#pragma once

#include <Windows.h>
#include <DirectXMath.h>

// Window class manages the main application window and its state.
// Design notes:
// - Small, well-documented API for window management.
// - Encapsulates Win32 window creation, message handling, and state.
class Window
{
public:
	// Initialize the window and register its class. May log/throw via engine error facilities.
	void Initialize();
	// Destroy the window and unregister its class.
	void Shutdown();
	// Poll and dispatch window messages (event loop). Non-blocking.
	void PollEvents() noexcept;

	HWND GetHWND() const noexcept { return m_windowHWND; }

	// Get the client rectangle of the window. Returns an empty rect if HWND is null.
	RECT GetRect() const noexcept;
	// Get the width/height of the window's client area.
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;

	DirectX::XMFLOAT2 GetViewportSize() const noexcept { return DirectX::XMFLOAT2(static_cast<float>(GetWidth()), static_cast<float>(GetHeight())); }
	DirectX::XMFLOAT2 GetViewportSizeInv() const noexcept { const auto s = GetViewportSize(); return DirectX::XMFLOAT2(s.x != 0.0f ? 1.0f / s.x : 0.0f, s.y != 0.0f ? 1.0f / s.y : 0.0f); }

	// Toggle fullscreen/windowed mode.
	void SetFullScreen(bool bSetFullScreen);

	bool ShouldClose() const noexcept { return m_bShouldClose; }
	bool IsFullScreen() const noexcept { return m_bIsFullScreen; }

private:
	// Window message handler (WndProc). Must match the `WNDPROC` signature exactly.
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	HWND m_windowHWND = nullptr;
	ATOM m_wndClass = 0;
	bool m_bShouldClose = false;
	bool m_bIsFullScreen = false;
};

// Global window instance
extern Window GWindow;