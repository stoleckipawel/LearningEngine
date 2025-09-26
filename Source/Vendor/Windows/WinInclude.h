#pragma once

//ToDo: precompiled header

#define NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include "Windows.h"
#include <cstdlib>
#include <iostream>
#include <string>

#include "d3d12.h"
#include "d3dx12.h"
#include <DirectXMath.h>
#include "dxgi1_6.h"
#include "D3Dcompiler.h"
#include "ComPointer.h"

#ifdef _DEBUG
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#endif

#include "wincodec.h"

using namespace DirectX; // directxmath library

enum ELogType
{
    Fatal,
    Warning,
    Info,
};


inline void LogError(const std::string& message, ELogType LogType)
{
    auto Icon = LogType == ELogType::Fatal ? MB_ICONERROR : MB_ICONWARNING;
    MessageBoxA(NULL, message.c_str(), "Error!", Icon | MB_OK);
    std::cerr << message << std::endl;
    if (LogType == ELogType::Fatal)
    {
        exit(EXIT_FAILURE);
    }
}

inline void ThrowIfFailed(HRESULT hr, const char* message, ELogType logType = ELogType::Fatal)
{
    if (FAILED(hr))
    {
        UINT icon = (logType == ELogType::Fatal) ? MB_ICONERROR : MB_ICONWARNING;
        char buf[256];
        sprintf_s(buf, "%s\nHRESULT 0x%08X", message, static_cast<unsigned int>(hr));

        MessageBoxA(nullptr, buf, "Error!", icon | MB_OK);
        std::cerr << buf << std::endl;

        if (logType == ELogType::Fatal)
        {
            exit(EXIT_FAILURE);
        }
    }
}


;