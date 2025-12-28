@echo off
:: ============================================================================
:: BuildSamplesDebug.bat - Debug configuration build wrapper
:: ============================================================================
:: Convenience wrapper for building samples in Debug configuration.
:: Delegates to the internal BuildSamplesImpl.bat implementation.
::
:: Usage: BuildSamplesDebug.bat
:: ============================================================================

setlocal

:: Logging bootstrap
if not defined LOG_CAPTURED (
    call "%~dp0internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: Delegate to implementation with Debug config
set "PARENT_BATCH=1"
call "%~dp0internal\BuildSamplesImpl.bat" Debug
set "RC=%ERRORLEVEL%"
set "PARENT_BATCH="

echo [LOG] BuildSamplesDebug.bat completed.

:: Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
set "_TMP_RC=%RC%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "BUILD_RC=%_TMP_RC%"

if defined PARENT_BATCH (
    exit /B %BUILD_RC%
)

echo.
echo [LOG] Logs: %LOGFILE%
pause
exit /B %BUILD_RC%
