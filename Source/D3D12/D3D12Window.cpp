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
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
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
	swd.BufferCount = GetFrameCount();
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

	if (!GetBuffers())
	{
		return false;
	}

	return true;
}

void D3D12Window::Shutdown()
{
	ReleaseBuffers();

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

void D3D12Window::Resize()
{
	ReleaseBuffers();

	RECT rect;
	if (GetClientRect(m_window, &rect))
	{
		m_width = rect.right - rect.left;
		m_height = rect.bottom - rect.top;
		m_swapChain->ResizeBuffers(GetFrameCount(),
			m_width, m_height,
			DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
			DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

		bShouldResize = false;
	};

	GetBuffers();
}

void D3D12Window::SetFullScreen(bool bSetFullScreen)
{
	//Update Window Styles
	DWORD style;
	DWORD exStyle;
	if (bSetFullScreen)
	{
		style = WS_POPUP | WS_VISIBLE;
		exStyle = WS_EX_APPWINDOW;
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
	}

	SetWindowLong(m_window, GWL_STYLE, style);
	SetWindowLong(m_window, GWL_EXSTYLE, exStyle);

	if (bSetFullScreen)
	{
		HMONITOR monitorFromWindow = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfoW(monitorFromWindow, &monitorInfo))
		{
			SetWindowPos(m_window, nullptr,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER);
		}
	}
	else
	{
		ShowWindow(m_window, SW_MAXIMIZE);
	}

	bIsFullScreen = bSetFullScreen;	
}

void D3D12Window::BeginFrame(ComPointer<ID3D12GraphicsCommandList7> cmdList)
{
	m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_buffers[m_currentBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	cmdList->ResourceBarrier(1, &barrier);
}

void D3D12Window::EndFrame(ComPointer<ID3D12GraphicsCommandList7> cmdList)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_buffers[m_currentBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	cmdList->ResourceBarrier(1, &barrier);
}

LRESULT D3D12Window::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_KEYDOWN:
		if (wParam == VK_F11)
		{
			//Toggle FullScreen  
			Get().SetFullScreen(!Get().IsFullScreen());
		}
		break;
	case WM_CLOSE:
		Get().bShouldClose = true;
		break;
	case WM_SIZE:
		if (lParam && (HIWORD(lParam) != Get().m_height && LOWORD(lParam) != Get().m_width))
		{
			Get().bShouldResize = true;
		}
		break;
    }
	return DefWindowProcW(wnd, msg, wParam, lParam);
}

bool D3D12Window::GetBuffers()
{
	//Get Swap Buffers
	for (size_t i = 0; i < m_FrameCount; i++)
	{
		if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]))))
		{
			return false;
		}
	}
	return true;
}

void D3D12Window::ReleaseBuffers()
{
	for (size_t i = 0; i < m_FrameCount; i++)
	{
		m_buffers[i].Release();
	}
}
