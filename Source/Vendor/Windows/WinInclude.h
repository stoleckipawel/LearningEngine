#pragma once

#define NOMINMAX

#include "Windows.h"
#include <cstdlib>
#include <iostream>

#include "d3d12.h"
#include <DirectXMath.h>
#include "dxgi1_6.h"
#include "ComPointer.h"

#ifdef _DEBUG
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#endif

#include "wincodec.h"

using namespace DirectX; // directxmath library

inline void LogError(const std::string& message)
{
    MessageBoxA(NULL, message.c_str(), "Critical Error", MB_ICONERROR | MB_OK);
    std::cerr << message << std::endl; // Log the error to console
    //exit(EXIT_FAILURE);
}


;