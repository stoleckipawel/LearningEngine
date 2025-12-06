#pragma once

// Window class manages the main application window and its state
class Window
{
public:
	// Initialize the window and register its class
	void Initialize();
	// Destroy the window and unregister its class
	void Shutdown();
	// Poll and dispatch window messages (event loop)
	void PollEvents();

	// Get the client rectangle of the window
	RECT GetRect();
	// Get the width of the window's client area
	UINT GetWidth();
	// Get the height of the window's client area
	UINT GetHeight();

	// Toggle fullscreen/windowed mode
	void SetFullScreen(bool bSetFullScreen);

	inline bool ShouldClose() { return m_bShouldClose; }
	inline bool IsFullScreen() { return m_bIsFullScreen; }
private:
	// Window message handler (WndProc)
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	// Handle to the window
	HWND WindowHWND = nullptr;
	ATOM m_wndClass = 0;
private:
	bool m_bShouldClose = false;
	bool m_bIsFullScreen = false;
};


// Global window instance
extern Window GWindow;