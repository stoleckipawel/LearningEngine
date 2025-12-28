@echo off
:: ============================================================================
:: BootstrapLog.bat - Centralized logging bootstrap for build toolchain
:: ============================================================================
:: Creates timestamped log files and captures all script output via PowerShell
:: Tee-Object. Invoked by all top-level scripts to ensure consistent logging.
::
:: Usage: call "BootstrapLog.bat" "<caller_full_path>" [args...]
::
:: Environment:
::   LOG_CAPTURED  - Set to "1" when logging is active (prevents re-entry)
::   LOGFILE       - Absolute path to the current session's log file
:: ============================================================================

setlocal

:: Guard: Already capturing - nothing to do
if defined LOG_CAPTURED (
    endlocal
    exit /B 0
)

:: Validate caller path argument
if "%~1"=="" (
    echo [ERROR] BootstrapLog.bat requires caller path as first argument.
    endlocal
    exit /B 1
)

set "CALLER=%~1"
shift

:: ---------------------------------------------------------------------------
:: Resolve repository root directory
:: ---------------------------------------------------------------------------
:: Prefer the caller's directory if it contains LICENSE.txt (repo root marker).
:: Otherwise fall back to this script's directory (tools\internal).
for %%F in ("%CALLER%") do set "CALLER_DIR=%%~dpF"

set "ROOT_DIR="
if exist "%CALLER_DIR%LICENSE.txt" set "ROOT_DIR=%CALLER_DIR%"
if not defined ROOT_DIR set "ROOT_DIR=%~dp0..\.."
:: Normalize to absolute path with trailing backslash
pushd "%ROOT_DIR%" >nul 2>&1
set "ROOT_DIR=%CD%\"
popd >nul

:: ---------------------------------------------------------------------------
:: Prepare logs directory and timestamped log file
:: ---------------------------------------------------------------------------
set "LOG_DIR=%ROOT_DIR%logs"
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%" >nul 2>&1

:: Generate timestamp: Day_MM-DD-YYYY__H-MM-SS-CC
set "TS=%DATE%_%TIME%"
set "TS=%TS::=-%"
set "TS=%TS:/=-%"
set "TS=%TS:,=-%"
set "TS=%TS:.=-%"
set "TS=%TS: =_%"
set "LOGFILE=%LOG_DIR%\logTools_%TS%.txt"

:: ---------------------------------------------------------------------------
:: Re-invoke caller under PowerShell Tee-Object for output capture
:: ---------------------------------------------------------------------------
:: Exports LOG_CAPTURED and LOGFILE so the re-invoked script skips bootstrap.
:: Use double-double quotes for CMD-safe escaping within PowerShell string.
powershell -NoProfile -ExecutionPolicy Bypass -Command "$env:LOG_CAPTURED='1'; $env:LOGFILE='%LOGFILE%'; & cmd /c '\""%CALLER%\"" %*' 2>&1 | Tee-Object -FilePath '%LOGFILE%'; exit $LASTEXITCODE"
set "RC=%ERRORLEVEL%"

:: Copy to a stable "latest" log for easy access
copy /Y "%LOGFILE%" "%LOG_DIR%\logTools.txt" >nul 2>&1

endlocal
exit /B %RC%
