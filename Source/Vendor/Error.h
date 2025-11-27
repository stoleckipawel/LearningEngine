
#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

// Log severity type
enum class ELogType
{
    Fatal,
    Warning,
    Info
};

// Log error message and optionally exit
inline void LogError(const std::string& message, ELogType logType)
{
    constexpr auto errorTitle = "Error!";
    UINT icon = (logType == ELogType::Fatal) ? MB_ICONERROR : MB_ICONWARNING;
    MessageBoxA(nullptr, message.c_str(), errorTitle, icon | MB_OK);
    std::cerr << message << std::endl;
    if (logType == ELogType::Fatal)
    {
        std::exit(EXIT_FAILURE);
    }
}

// Throw if HRESULT failed, log error and optionally exit
inline void ThrowIfFailed(HRESULT hr, const char* message, ELogType logType = ELogType::Fatal)
{
    if (FAILED(hr))
    {
        constexpr auto errorTitle = "Error!";
        UINT icon = (logType == ELogType::Fatal) ? MB_ICONERROR : MB_ICONWARNING;
        char buf[256];
        std::snprintf(buf, sizeof(buf), "%s\nHRESULT 0x%08X", message, static_cast<unsigned int>(hr));
        MessageBoxA(nullptr, buf, errorTitle, icon | MB_OK);
        std::cerr << buf << std::endl;
        if (logType == ELogType::Fatal)
        {
            std::exit(EXIT_FAILURE);
        }
    }
}