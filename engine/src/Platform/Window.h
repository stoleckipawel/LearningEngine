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
	void Initialize();
	void Shutdown();

	void PollEvents() noexcept;

	HWND GetHWND() const noexcept { return m_windowHWND; }

	RECT GetRect() const noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;

	DirectX::XMFLOAT2 GetViewportSize() const noexcept;
	DirectX::XMFLOAT2 GetViewportSizeInv() const noexcept;

	void SetFullScreen(bool bSetFullScreen);
	bool ShouldClose() const noexcept { return m_bShouldClose; }
	bool IsFullScreen() const noexcept { return m_bIsFullScreen; }

  private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

  private:
	HWND m_windowHWND = nullptr;
	ATOM m_wndClass = 0;
	bool m_bShouldClose = false;
	bool m_bIsFullScreen = false;
};

// Global window instance
extern Window GWindow;