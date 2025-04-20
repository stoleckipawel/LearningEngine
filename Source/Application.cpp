#include <cstdio>
#include <iostream>
#include "Vendor/Windows/WinInclude.h"
#include "Debug/D3D12DebugLayer.h"

int main()
{
    D3D12DebugLayer::Get().Initialize();

	POINT CursorPos;
    GetCursorPos(&CursorPos);

    std::cout << "The cursor is x: " << CursorPos.x << " y: " << CursorPos.y;

	D3D12DebugLayer::Get().Shutdown();

    return 0;
}