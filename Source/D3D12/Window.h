#pragma once
#include "../Vendor/Windows/WinInclude.h"

class Window
{
public:
	bool Initialize();
	void Shutdown();
	void Update();

	RECT GetRect();
	UINT GetWidth();
	UINT GetHeight();

	void SetFullScreen(bool bSetFullScreen);
	inline bool ShouldClose() { return m_bShouldClose; };
	inline bool IsFullScreen() { return m_bIsFullScreen; };
private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	HWND WindowHWND = nullptr;
private:
	bool m_bShouldClose = false;
	bool m_bIsFullScreen = false;
	ATOM m_wndClass;
};

extern Window GWindow;