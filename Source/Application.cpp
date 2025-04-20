#include <iostream>

#include "Vendor/Windows/WinInclude.h"

#include "Debug/D3D12DebugLayer.h"
#include "D3D12/D3D12Context.h"

int main()
{
    D3D12DebugLayer::Get().Initialize();

	if(D3D12Context::Get().Initialize());
	{
		D3D12Context::Get().GetDevice();
		D3D12Context::Get().GetQueue();
		D3D12Context::Get().GetFence();

		D3D12Context::Get().Shutdown();
	}

	D3D12DebugLayer::Get().Shutdown();

    return 0;
}