#include <iostream>

#include "Vendor/Windows/WinInclude.h"

#include "Debug/D3D12DebugLayer.h"
#include "D3D12/D3D12Context.h"

int main()
{
    D3D12DebugLayer::Get().Initialize();

	if(D3D12Context::Get().Initialize());
	{
		while (true)
		{
			auto cmdList = D3D12Context::Get().InitializeCommandList();
			D3D12Context::Get().ExecuteCommandList();
		}

		D3D12Context::Get().Shutdown();
	}

	D3D12DebugLayer::Get().Shutdown();

    return 0;
}