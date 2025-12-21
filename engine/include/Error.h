
#pragma once

// -----------------------------------------------------------------------------
// Error/log utilities
//
// Goals:
// - Every log/error includes the call-site (file:line)
// -----------------------------------------------------------------------------

#include <Windows.h>

#include <cstdint>
#include <string_view>

// -----------------------------------------------------------------------------
// Feature detection: use `std::source_location` when possible
//
// Why this exists:
// - We want `LogMessage(...)` / `ThrowIfFailed(...)` to automatically capture the
//   *call site* (file + line) without you passing it manually.
// - C++20 provides this via `std::source_location`.
// - However, some setups (especially IntelliSense vs build toolchain) may not
//   expose `<source_location>` even when the project otherwise compiles.
//
// So we decide at compile time:
// - If C++20 is enabled AND `<source_location>` exists => use it.
// - Otherwise => fall back to `__FILE__` / `__LINE__` macros.
// -----------------------------------------------------------------------------

// The C++ language version macro differs:
// - MSVC: `_MSVC_LANG` is the reliable one
// - Others: `__cplusplus`
#if defined(_MSVC_LANG)
    #define LE_CPLUSPLUS_VERSION _MSVC_LANG
#else
    #define LE_CPLUSPLUS_VERSION __cplusplus
#endif

#if (LE_CPLUSPLUS_VERSION >= 202002L) && defined(__has_include) && __has_include(<source_location>)
    #include <source_location>
    #define LE_USE_STD_SOURCE_LOCATION 1
#endif

// Log severity type
enum class ELogType
{
    Fatal,
    Warning,
    Info
};

namespace ErrorDetail
{
    // Minimal location info we need for logging.
    // Stored as raw file pointer + line to keep the API lightweight.
    struct SourceLocation
    {
        const char* file = "";
        std::uint32_t line = 0;
    };

    [[nodiscard]] constexpr SourceLocation MakeLocation(const char* file, std::uint32_t line)
    {
        return SourceLocation{ file, line };
    }

#if LE_USE_STD_SOURCE_LOCATION
    [[nodiscard]] constexpr SourceLocation ToLocation(const std::source_location& location)
    {
        return SourceLocation{ location.file_name(), static_cast<std::uint32_t>(location.line()) };
    }
#endif
}

// -----------------------------------------------------------------------------
// Implementation entry points (defined in Error.cpp)
// -----------------------------------------------------------------------------

// Shows an error message and returns the user's choice.
// Return values: IDOK/IDRETRY/IDCANCEL depending on buttons shown.
[[nodiscard]] int ShowErrorMessageAt(std::string_view msg, ELogType logType, ErrorDetail::SourceLocation location) noexcept;

// Logs a message (and for Fatal: may exit on Cancel).
void LogMessageAt(std::string_view message, ELogType logType, ErrorDetail::SourceLocation location) noexcept;

// If `hr` indicates failure, logs as Fatal (may exit on Cancel).
// Callers should use `ThrowIfFailed(hr, "message")`.
void ThrowIfFailedAt(HRESULT hr, std::string_view message, ErrorDetail::SourceLocation location) noexcept;

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

#if LE_USE_STD_SOURCE_LOCATION

// Preferred path: call-site capture via C++20 std::source_location.

[[nodiscard]] inline int ShowErrorMessage(
    std::string_view msg,
    ELogType logType,
    const std::source_location& location = std::source_location::current())
{
    return ShowErrorMessageAt(msg, logType, ErrorDetail::ToLocation(location));
}

inline void LogMessage(
    std::string_view message,
    ELogType logType = ELogType::Info,
    const std::source_location& location = std::source_location::current())
{
    LogMessageAt(message, logType, ErrorDetail::ToLocation(location));
}

inline void ThrowIfFailed(
    HRESULT hr,
    std::string_view message,
    const std::source_location& location = std::source_location::current())
{
    ThrowIfFailedAt(hr, message, ErrorDetail::ToLocation(location));
}

#else

// Fallback path: call-site capture via __FILE__/__LINE__ macros.
// We provide macros that forward into the *At functions implemented in Error.cpp.

// LogMessage("hi")
#define LE_LOGMESSAGE_1(message) \
    ::LogMessageAt((message), ELogType::Info, ::ErrorDetail::MakeLocation(__FILE__, static_cast<std::uint32_t>(__LINE__)))

// LogMessage("hi", ELogType::Warning)
#define LE_LOGMESSAGE_2(message, logType) \
    ::LogMessageAt((message), (logType), ::ErrorDetail::MakeLocation(__FILE__, static_cast<std::uint32_t>(__LINE__)))

#define LE_LOGMESSAGE_GET_MACRO(_1, _2, NAME, ...) NAME
#define LogMessage(...) LE_LOGMESSAGE_GET_MACRO(__VA_ARGS__, LE_LOGMESSAGE_2, LE_LOGMESSAGE_1)(__VA_ARGS__)

// ThrowIfFailed(hr, "msg") -> call-site captured via macros
#define ThrowIfFailed(hr, message) \
    ::ThrowIfFailedAt((hr), (message), ::ErrorDetail::MakeLocation(__FILE__, static_cast<std::uint32_t>(__LINE__)))

#endif