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

	void BeginFrame(ComPointer<ID3D12GraphicsCommandList7> cmdList);
	void EndFrame(ComPointer<ID3D12GraphicsCommandList7> cmdList);

	inline bool GetShouldClose() { return bShouldClose; };
	inline bool GetShouldResize() { return bShouldResize; };
	inline bool IsFullScreen() { return bIsFullScreen; };
	inline UINT GetWidth() { return m_width; }
	inline UINT GetHeight() { return m_height; }
	inline UINT GetCurrentBufferIndex() { return m_currentBufferIndex; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackbufferRTVHandle() { return m_rtvHandles[m_currentBufferIndex]; }

	D3D12_VIEWPORT GetDefaultViewport();
	D3D12_RECT GetDefaultScissorRect();

	static constexpr UINT GetFrameCount() { return m_FrameCount; };//2:Vsync OFF, 3: Vsync ON
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
	static constexpr UINT m_FrameCount = 2;
	UINT m_currentBufferIndex = 0;

	ComPointer<ID3D12DescriptorHeap> m_rtvDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[m_FrameCount];

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