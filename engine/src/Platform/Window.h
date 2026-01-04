#pragma once

#include <Windows.h>
#include <DirectXMath.h>

// Window class manages the main application window and its state.
// Design notes:
// - Small, well-documented API for window management.
// - Encapsulates Win32 window creation, message handling, and state.
class Window final
{
  public:
	[[nodiscard]] static Window& Get() noexcept;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	void Initialize();
	void Shutdown();

	void PollEvents() noexcept;

	[[nodiscard]] HWND GetHWND() const noexcept { return m_windowHWND; }

	[[nodiscard]] RECT GetRect() const noexcept;
	[[nodiscard]] UINT GetWidth() const noexcept;
	[[nodiscard]] UINT GetHeight() const noexcept;

	[[nodiscard]] DirectX::XMFLOAT2 GetViewportSize() const noexcept;
	[[nodiscard]] DirectX::XMFLOAT2 GetViewportSizeInv() const noexcept;

	void SetFullScreen(bool bSetFullScreen);
	[[nodiscard]] bool ShouldClose() const noexcept { return m_bShouldClose; }
	[[nodiscard]] bool IsFullScreen() const noexcept { return m_bIsFullScreen; }

  private:
	Window() = default;
	~Window() = default;

	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND m_windowHWND = nullptr;
	ATOM m_wndClass = 0;
	bool m_bShouldClose = false;
	bool m_bIsFullScreen = false;
};

inline Window& GWindow = Window::Get();