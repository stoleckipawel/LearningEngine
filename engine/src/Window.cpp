#include "PCH.h"
#include "Window.h"
#include "Renderer.h"
#include "RHI.h"
#include "UI.h"
#include "EngineConfig.h"
#include <string>

Window GWindow;

RECT Window::GetRect() const noexcept
{
	RECT rect{};
	if (m_windowHWND)
	{
		GetClientRect(m_windowHWND, &rect);
	}
	return rect;
}

UINT Window::GetWidth() const noexcept
{
	RECT rect = GetRect();
	return rect.right - rect.left;
}

UINT Window::GetHeight() const noexcept
{
	RECT rect = GetRect();
	return rect.bottom - rect.top;
}


// Initializes the window and registers its class
void Window::Initialize()
{
	// Create window class description
	WNDCLASSEXW windowClass{ 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC; // Own device context
	windowClass.lpfnWndProc = &Window::OnWindowMessage;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"Default Window Name";
	windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	// Register window class
	m_wndClass = RegisterClassExW(&windowClass);
	if (!m_wndClass)
	{
		LOG_FATAL("Failed to register window class");
	}

	// Convert configured window title to wide string
	std::wstring wtitle(EngineSettings::WindowTitle.begin(), EngineSettings::WindowTitle.end());

	// Create the window with configured initial size
	m_windowHWND = CreateWindowExW(
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
		L"Default Window Name", // class name string
		wtitle.c_str(),          // window name string
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	if (m_windowHWND == nullptr)
	{
		LOG_FATAL("Failed to Create a Window");
	}

	// Apply fullscreen if requested by settings
	if (EngineSettings::StartFullscreen)
	{
		SetFullScreen(true);
	}
}


// Shuts down the window and unregisters its class
void Window::Shutdown()
{
	if (m_windowHWND)
	{
		DestroyWindow(m_windowHWND);
	}

	if (m_wndClass)
	{
		UnregisterClassW(L"Default Window Name", GetModuleHandle(nullptr));
		m_wndClass = 0;
	}
}


// Polls and dispatches pending window messages (event loop)
void Window::PollEvents() noexcept
{
	MSG msg;
	while (PeekMessageW(&msg, m_windowHWND, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}


// Sets the window to fullscreen or windowed mode
void Window::SetFullScreen(bool bSetFullScreen)
{
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

	SetWindowLongW(m_windowHWND, GWL_STYLE, style);
	SetWindowLongW(m_windowHWND, GWL_EXSTYLE, exStyle);

	if (bSetFullScreen)
	{
		// Resize window to cover the monitor
		HMONITOR monitorFromWindow = MonitorFromWindow(m_windowHWND, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfoW(monitorFromWindow, &monitorInfo))
		{
			SetWindowPos(m_windowHWND, nullptr,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER);
		}
	}
	else
	{
		// Maximize window in windowed mode
		ShowWindow(m_windowHWND, SW_MAXIMIZE);
	}

	m_bIsFullScreen = bSetFullScreen;    
}



// Window message handler (WndProc)
LRESULT Window::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#if USE_GUI
	if (GUI.OnWindowMessage(wnd, msg, wParam, lParam))
#endif
	{
		//GUI Layer enforces being first & if it returns true we must stop processing further events
		return true;
	}

	switch (msg)
	{
		case WM_KEYDOWN:
			// Toggle fullscreen on F11
			if (wParam == VK_F11)
			{
				GWindow.SetFullScreen(!GWindow.IsFullScreen());
			}
			return 0;
		case WM_SIZE:
			// Trigger resize when the window is not minimized and device is ready
			if (wParam != SIZE_MINIMIZED && GRHI.GetDevice() != nullptr)
			{
				GRenderer.OnResize();
			}
			return 0;
		case WM_CLOSE:
		case WM_QUIT:
			// Signal to close the window
			GWindow.m_bShouldClose = true;
			return 0;
	}
	// Default message handling
	return DefWindowProcW(wnd, msg, wParam, lParam);
}


