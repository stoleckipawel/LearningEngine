#include "Error.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <string_view>

namespace
{
    [[nodiscard]] std::string_view GetFileName(std::string_view path)
    {
        const size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash == std::string_view::npos)
            return path;
        return path.substr(lastSlash + 1);
    }

    [[nodiscard]] std::string FormatSourcePrefix(ErrorDetail::SourceLocation location)
    {
        const std::string_view filePath = (location.file != nullptr) ? std::string_view(location.file) : std::string_view();
        const std::string_view fileName = GetFileName(filePath);

        // `fileName` is a string_view (not guaranteed null-terminated), so we use "%.*s".
        char buf[512];
        std::snprintf(
            buf,
            sizeof(buf),
            "%.*s:%u: ",
            static_cast<int>(fileName.size()),
            fileName.data(),
            static_cast<unsigned int>(location.line));
        return std::string(buf);
    }

    [[nodiscard]] constexpr const char* GetSeverityPrefix(ELogType logType)
    {
        switch (logType)
        {
        case ELogType::Fatal:   return "[FATAL] ";
        case ELogType::Warning: return "[WARNING] ";
        case ELogType::Info:    return "[INFO] ";
        }
        return "";
    }

    [[nodiscard]] constexpr const char* GetErrorTitle(ELogType logType)
    {
        switch (logType)
        {
        case ELogType::Fatal:   return "Fatal Error";
        case ELogType::Warning: return "Warning";
        case ELogType::Info:    return "Info";
        }
        return "Error";
    }

    [[nodiscard]] constexpr UINT GetErrorIcon(ELogType logType)
    {
        switch (logType)
        {
        case ELogType::Fatal:   return MB_ICONERROR;
        case ELogType::Warning: return MB_ICONWARNING;
        case ELogType::Info:    return MB_ICONINFORMATION;
        }
        return MB_ICONERROR;
    }

    [[nodiscard]] std::string FormatMessageWithLocation(std::string_view message, ErrorDetail::SourceLocation location)
    {
        const std::string prefix = FormatSourcePrefix(location);
        return prefix + std::string(message);
    }
}

int ShowErrorMessageAt(std::string_view msg, ELogType logType, ErrorDetail::SourceLocation location)
{
    const std::string msgWithLocation = FormatMessageWithLocation(msg, location);

    // Always emit to stderr (useful when MessageBox is suppressed/hidden).
    std::cerr << msgWithLocation << std::endl;

#if defined(_DEBUG)
    // Trigger a debugger breakpoint first in debug builds.
    if (IsDebuggerPresent())
        DebugBreak();
#endif

    UINT flags = GetErrorIcon(logType);
    flags |= (logType == ELogType::Fatal) ? MB_RETRYCANCEL : MB_OK;

    return MessageBoxA(nullptr, msgWithLocation.c_str(), GetErrorTitle(logType), flags);
}

void LogMessageAt(std::string_view message, ELogType logType, ErrorDetail::SourceLocation location)
{
    const std::string formatted = std::string(GetSeverityPrefix(logType)) + std::string(message);
    const int choice = ShowErrorMessageAt(formatted, logType, location);

    if (logType == ELogType::Fatal)
    {
        // For Fatal we use MB_RETRYCANCEL.
        // Cancel => exit; Retry => continue.
        if (choice == IDCANCEL)
            std::exit(EXIT_FAILURE);
    }
}

void ThrowIfFailedAt(HRESULT hr, std::string_view message, ErrorDetail::SourceLocation location)
{
    if (!FAILED(hr))
        return;

    char hrBuf[64];
    std::snprintf(hrBuf, sizeof(hrBuf), "HRESULT 0x%08X", static_cast<unsigned int>(hr));

    std::string combined;
    combined.reserve(message.size() + 1 + 32);
    combined.append(message.data(), message.size());
    combined.push_back('\n');
    combined += hrBuf;

    LogMessageAt(combined, ELogType::Fatal, location);
}
