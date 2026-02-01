@echo off
:: ============================================================================
:: CheckDependencies.bat - Build toolchain dependency validator
:: ============================================================================
:: Verifies required and optional build tools are available in PATH.
::
:: Required: CMake, MSBuild
:: Optional: Clang (for ClangCL), clang-format, clang-tidy
::
:: Usage: CheckDependencies.bat [CONTINUE]
::   CONTINUE - Suppress pause (used by parent scripts)
::
:: Environment:
::   PARENT_BATCH  - When set, suppresses pause on completion
::   LOG_CAPTURED  - Indicates logging is already active
::   LOGFILE       - Path to current log file
::
:: Exit Codes:
::   0 - All required dependencies found
::   1 - One or more required dependencies missing
:: ============================================================================

setlocal

:: ---------------------------------------------------------------------------
:: Logging bootstrap
:: ---------------------------------------------------------------------------
if not defined LOG_CAPTURED (
    call "%~dp0Tools\Internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: Track overall status (0 = success)
set "RC=0"

echo.
echo ============================================================
echo   Validating Build Dependencies
echo ============================================================
echo.

:: ---------------------------------------------------------------------------
:: Required: CMake
:: ---------------------------------------------------------------------------
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Install CMake and add to PATH.
    set "RC=1"
) else (
    echo [OK] CMake
)

:: ---------------------------------------------------------------------------
:: Required: MSBuild
:: ---------------------------------------------------------------------------
where msbuild >nul 2>&1
if errorlevel 1 (
    echo [ERROR] MSBuild not found. Install Visual Studio C++ workload.
    set "RC=1"
) else (
    echo [OK] MSBuild
)

:: ---------------------------------------------------------------------------
:: Optional: Clang (enables ClangCL toolset)
:: ---------------------------------------------------------------------------
where clang >nul 2>&1
if errorlevel 1 (
    echo [WARN] Clang not found. Will use MSVC toolset.
) else (
    echo [OK] Clang (ClangCL toolset available)
)

:: ---------------------------------------------------------------------------
:: Optional: clang-format
:: ---------------------------------------------------------------------------
where clang-format >nul 2>&1
if errorlevel 1 (
    echo [WARN] clang-format not found. Code formatting unavailable.
) else (
    echo [OK] clang-format
)

:: ---------------------------------------------------------------------------
:: Optional: clang-tidy
:: ---------------------------------------------------------------------------
where clang-tidy >nul 2>&1
if errorlevel 1 (
    echo [WARN] clang-tidy not found. Static analysis unavailable.
) else (
    echo [OK] clang-tidy
)

:: ---------------------------------------------------------------------------
:: Summary
:: ---------------------------------------------------------------------------
echo.
echo ============================================================
if %RC%==0 (
    echo   [SUCCESS] All required dependencies found.
) else (
    echo   [ERROR] Missing required dependencies. Build will fail.
)
echo ============================================================

:: Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
set "_TMP_RC=%RC%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "DEP_RC=%_TMP_RC%"

if defined PARENT_BATCH (
    exit /B %DEP_RC%
)

echo.
echo [LOG] Logs: %LOGFILE%
pause
exit /B %DEP_RC%
