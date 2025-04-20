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

	return m_window != nullptr;
}

void D3D12Window::Shutdown()
{
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
