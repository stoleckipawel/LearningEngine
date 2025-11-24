#include "Window.h"
#include "Renderer.h"

Window GWindow;

RECT Window::GetRect()
{
	RECT rect;
	GetClientRect(WindowHWND, &rect);
	return rect;
}

UINT Window::GetWidth()
{
	RECT rect = GetRect();
	return rect.right - rect.left;
}

UINT Window::GetHeight()
{
	RECT rect = GetRect();
	return rect.bottom - rect.top;
}

bool Window::Initialize()
{
	//Create Win Window Desc
	WNDCLASSEXW windowClass{ 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;// own device context
	windowClass.lpfnWndProc = &Window::OnWindowMessage;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"Default Window Name";
	windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	//Register Win Window Class
	m_wndClass = RegisterClassExW(&windowClass);
	if (!m_wndClass)
	{
		std::string message = "Window: Failed to register window class";
		LogError(message, ELogType::Fatal);
		return false;
	}

	//Create Win Window
	WindowHWND = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
		(LPCWSTR)m_wndClass,
		L"Engine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	if (WindowHWND == nullptr)
	{
		std::string message = "Window: Failed to Create a Window";
		LogError(message, ELogType::Fatal);
		return false;
	}

	return true;
}

void Window::Shutdown()
{
	if (WindowHWND)
	{
		DestroyWindow(WindowHWND);
	}

	if (m_wndClass)
	{
		UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandle(nullptr));
		m_wndClass = 0;
	}
}

void Window::Update()
{
	// Process pending window messages
	MSG msg;
	while (PeekMessageW(&msg, WindowHWND, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void Window::SetFullScreen(bool bSetFullScreen)
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

	SetWindowLongW(WindowHWND, GWL_STYLE, style);
	SetWindowLongW(WindowHWND, GWL_EXSTYLE, exStyle);

	if (bSetFullScreen)
	{
		HMONITOR monitorFromWindow = MonitorFromWindow(WindowHWND, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfoW(monitorFromWindow, &monitorInfo))
		{
			SetWindowPos(WindowHWND, nullptr,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER);
		}
	}
	else
	{
		ShowWindow(WindowHWND, SW_MAXIMIZE);
	}

	m_bIsFullScreen = bSetFullScreen;	
}


LRESULT Window::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
		case WM_KEYDOWN:
			if (wParam == VK_F11)
			{
				GWindow.SetFullScreen(!GWindow.IsFullScreen());
			}
			return 0;
		case WM_SIZE:
			if (lParam && (HIWORD(lParam) != GWindow.GetHeight() && LOWORD(lParam) != GWindow.GetWidth()))
			{
				if (GRHI.Device != nullptr)
				{
					GRenderer.OnResize();
				}
			}	
			return 0;
		case WM_CLOSE:
		case WM_QUIT:
			GWindow.m_bShouldClose = true;
			return 0;
    }
	return DefWindowProcW(wnd, msg, wParam, lParam);
}


