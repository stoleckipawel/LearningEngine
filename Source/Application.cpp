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
		while (true)
		{
			auto cmdList = D3D12Context::Get().InitializeCommandList();

			//a lot of setup

			//draw

			D3D12Context::Get().ExecuteCommandList();
		}

		D3D12Context::Get().Shutdown();
	}

	D3D12DebugLayer::Get().Shutdown();

    return 0;
}