#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"

class D3D12Window
{
public:
	bool Initialize();
	void Shutdown();
	void Update();
	void Present();
	inline bool GetShouldClose() { return bShouldClose; };

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	ATOM m_wndClass;
	HWND m_window = nullptr;
	bool bShouldClose = false;

	ComPointer<IDXGISwapChain3> m_swapChain;

	//Singleton pattern to ensure only one instance of the debug layer exists
public:
	D3D12Window(const D3D12Window&) = delete;
	D3D12Window& operator=(const D3D12Window&) = delete;

	inline static D3D12Window& Get()
	{
		static D3D12Window instance;
		return instance;
	}
private:
	D3D12Window() = default;
};