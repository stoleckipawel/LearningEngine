

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

// Helper to show message box and log
inline void ShowErrorMessage(const std::string& msg, ELogType logType) 
{
    std::cerr << msg << std::endl;
    MessageBoxA(nullptr, msg.c_str(), GetErrorTitle(logType), GetErrorIcon(logType) | MB_OK);
}

// Log message and optionally exit
inline void LogMessage(const std::string& message, ELogType logType = ELogType::Info)
{
    std::string formattedMsg = GetSeverityPrefix(logType) + message;
    ShowErrorMessage(formattedMsg, logType);
    if (logType == ELogType::Fatal)
        std::exit(EXIT_FAILURE);
}

// Throw if HRESULT failed, log error and optionally exit
inline void ThrowIfFailed(HRESULT hr, const char* message, ELogType logType = ELogType::Fatal)
{
    if (FAILED(hr))
    {
        char buf[512];
        std::snprintf(buf, sizeof(buf), "%s\nHRESULT 0x%08X", message, static_cast<unsigned int>(hr));
        std::string formattedMsg = std::string(GetSeverityPrefix(logType)) + std::string(buf);
        ShowErrorMessage(formattedMsg, logType);
        if (logType == ELogType::Fatal)
            std::exit(EXIT_FAILURE);
    }
}