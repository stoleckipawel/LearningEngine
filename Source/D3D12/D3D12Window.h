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
	void Resize();
	void SetFullScreen(bool bSetFullScreen);

	inline bool GetShouldClose() { return bShouldClose; };
	inline bool GetShouldResize() { return bShouldResize; };
	inline bool IsFullScreen() { return bIsFullScreen; };

	static constexpr size_t GetFrameCount() { return m_FrameCount; };//2:Vsync OFF, 3: Vsync ON
private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool GetBuffers();
	void ReleaseBuffers();
private:
	ATOM m_wndClass;
	HWND m_window = nullptr;
	bool bShouldClose = false;
	bool bShouldResize = false;
	bool bIsFullScreen = false;
	UINT m_width = 1;
	UINT m_height = 1;
	static constexpr size_t m_FrameCount = 2;

	ComPointer<IDXGISwapChain3> m_swapChain;
	ComPointer<ID3D12Resource2> m_buffers[m_FrameCount];

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