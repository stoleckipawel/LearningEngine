#include "D3D12Window.h"

bool D3D12Window::Initialize()
{
	WNDCLASSEXW windowClassX{};
    windowClassX.cbSize = sizeof(windowClassX);
    windowClassX.style = CS_OWNDC;// own device context
    windowClassX.lpfnWndProc = &D3D12Window::OnWindowMessage;
    windowClassX.cbClsExtra = 0;
    windowClassX.cbWndExtra = 0;
    windowClassX.hInstance = GetModuleHandle(nullptr);
	windowClassX.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClassX.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClassX.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClassX.lpszMenuName = nullptr;
	windowClassX.lpszClassName = L"Default Window Name";
	windowClassX.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	m_wndClass = RegisterClassExW(&windowClassX);

	if (!m_wndClass)
	{
		MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_window = CreateWindowEx(
		0,
		(LPCWSTR)m_wndClass,
		L"Unrealistic Engine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		windowClassX.hInstance,
		nullptr
	);

	if (m_window == nullptr)
	{
		return false;
	}

	//Describe Swapchain
	DXGI_SWAP_CHAIN_DESC1 swd{};

	RECT rect;
	GetClientRect(m_window, &rect);
	swd.Width = rect.right - rect.left;
	swd.Height = rect.bottom - rect.top;
	swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swd.Stereo = false;
	swd.SampleDesc.Quality = 0;
	swd.SampleDesc.Count = 1;
	swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swd.BufferCount = 2;//2:Vsync OFF, 3: Vsync ON
	swd.Scaling = DXGI_SCALING_STRETCH;
	swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};
	sfd.Windowed = true;

	//Swapchain Creation
	auto& factory = D3D12Context::Get().GetFactory();
	ComPointer<IDXGISwapChain1> sc1;
	factory->CreateSwapChainForHwnd(D3D12Context::Get().GetCommmandQueue(), m_window, &swd, &sfd, nullptr, &sc1);
	if (!sc1.QueryInterface(m_swapChain))
	{
		return false;
	}

	return true;

}

void D3D12Window::Shutdown()
{
	m_swapChain.Release();

	if (m_window)
	{
		DestroyWindow(m_window);
	}

	if (m_wndClass)
	{
		UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandle(nullptr));
		m_wndClass = 0;
	}
}

void D3D12Window::Update()
{
	MSG msg;
	while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void D3D12Window::Present()
{
	m_swapChain->Present(1, 0);
}

LRESULT D3D12Window::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_CLOSE:
		Get().bShouldClose = true;
		break;
    }
	return DefWindowProcW(wnd, msg, wParam, lParam);
}
