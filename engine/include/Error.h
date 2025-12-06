

// Log severity type
enum class ELogType
{
    Fatal,
    Warning,
    Info
};

// Helper to get severity prefix
inline const char* GetSeverityPrefix(ELogType logType) 
{
    switch (logType) {
        case ELogType::Fatal:   return "[FATAL] ";
        case ELogType::Warning: return "[WARNING] ";
        case ELogType::Info:    return "[INFO] ";
    }
    return "";
}

// Helper to get window title
inline const char* GetErrorTitle(ELogType logType) 
{
    switch (logType) {
        case ELogType::Fatal:   return "Fatal Error";
        case ELogType::Warning: return "Warning";
        case ELogType::Info:    return "Info";
    }
    return "Error";
}

// Helper to get icon
inline UINT GetErrorIcon(ELogType logType) 
{
    switch (logType) {
        case ELogType::Fatal:   return MB_ICONERROR;
        case ELogType::Warning: return MB_ICONWARNING;
        case ELogType::Info:    return MB_ICONINFORMATION;
    }
    return MB_ICONERROR;
}


// Shows an error message, triggers breakpoint in debug, and returns user's choice.
// Return values: IDOK/IDRETRY/IDCANCEL depending on buttons shown.
inline int ShowErrorMessage(const std::string& msg, ELogType logType) 
{
    std::cerr << msg << std::endl;
#if defined(_DEBUG)
    // Trigger a debugger breakpoint first in debug builds
    if (IsDebuggerPresent())
        DebugBreak();
#endif

    UINT flags = GetErrorIcon(logType);
    int choice = IDOK;
    if (logType == ELogType::Fatal)
    {
        // Allow user to choose whether to Retry (continue) or Cancel (exit)
        flags |= MB_RETRYCANCEL;
        choice = MessageBoxA(nullptr, msg.c_str(), GetErrorTitle(logType), flags);
    }
    else
    {
        flags |= MB_OK;
        choice = MessageBoxA(nullptr, msg.c_str(), GetErrorTitle(logType), flags);
    }

    return choice;
}

// Log message and optionally exit
inline void LogMessage(const std::string& message, ELogType logType = ELogType::Info)
{
    std::string formattedMsg = GetSeverityPrefix(logType) + message;
    int choice = ShowErrorMessage(formattedMsg, logType);
    if (logType == ELogType::Fatal)
    {
        if (choice == IDCANCEL)
            std::exit(EXIT_FAILURE);
        // If Retry selected, continue execution.
    }
}

// Throw if HRESULT failed, log error and optionally exit
inline void ThrowIfFailed(HRESULT hr, const char* message, ELogType logType = ELogType::Fatal)
{
    if (FAILED(hr))
    {
        char buf[512];
        std::snprintf(buf, sizeof(buf), "%s\nHRESULT 0x%08X", message, static_cast<unsigned int>(hr));
        std::string formattedMsg = std::string(GetSeverityPrefix(logType)) + std::string(buf);
        int choice = ShowErrorMessage(formattedMsg, logType);
        if (logType == ELogType::Fatal)
        {
            if (choice == IDCANCEL)
                std::exit(EXIT_FAILURE);
            // Retry continues execution so caller can attempt recovery or re-run
        }
    }
}