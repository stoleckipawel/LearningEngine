#include <iostream>

#include "Vendor/Windows/WinInclude.h"

#include "Debug/D3D12DebugLayer.h"
#include "D3D12/D3D12Context.h"
#include "D3D12/D3D12Window.h"

int main()
{
    D3D12DebugLayer::Get().Initialize();

	if(D3D12Context::Get().Initialize() && D3D12Window::Get().Initialize());
	{
		D3D12Window::Get().SetFullScreen(true);

		while (!D3D12Window::Get().GetShouldClose())
		{
			// Process pending window messages
			D3D12Window::Get().Update();

			//Handle Resizing
			if (D3D12Window::Get().GetShouldResize())
			{
				D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());
				D3D12Window::Get().Resize();
			}

			//Begin Drawing
			auto cmdList = D3D12Context::Get().InitializeCommandList();

			//ToDo: draw

			//Finish Drawing & Present
			D3D12Context::Get().ExecuteCommandList();
			D3D12Window::Get().Present();
		}

		
		D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());

		D3D12Window::Get().Shutdown();
		D3D12Context::Get().Shutdown();
	}

	D3D12DebugLayer::Get().Shutdown();

    return 0;
}