#pragma once
#include "../Vendor/Windows/WinInclude.h"

class FWindow
{
public:
	bool Initialize();
	void Shutdown();
	void Update();

	RECT GetRect();
	UINT GetWidth();
	UINT GetHeight();

	void SetFullScreen(bool bSetFullScreen);

	inline bool ShouldClose() { return bShouldClose; };
	inline bool IsFullScreen() { return bIsFullScreen; };

	ATOM m_wndClass;
	HWND m_window = nullptr;
private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	bool bShouldClose = false;
	bool bIsFullScreen = false;
};

extern FWindow GWindow;