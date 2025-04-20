#include <cstdio>
#include <iostream>
#include "Vendor/Windows/WinInclude.h"

int main()
{
    ComPointer<IUnknown> p;


	POINT CursorPos;
    GetCursorPos(&CursorPos);

    std::cout << "The cursor is x: " << CursorPos.x << " y: " << CursorPos.y;
    return 0;
}