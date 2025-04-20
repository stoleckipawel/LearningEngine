#include <cstdio>
#include <iostream>
#include "Vendor/WinInclude.h"

int main()
{
    
	POINT CursorPos;
    GetCursorPos(&CursorPos);

    std::cout << "The cursor is x: " << CursorPos.x << " y: " << CursorPos.y;
    return 0;
}