@echo off
:: ============================================================================
:: BuildProjectsDebug.bat - Debug configuration build wrapper
:: ============================================================================
:: Convenience wrapper for building projects in Debug configuration.
:: Delegates to the internal BuildProjectsImpl.bat implementation.
::
:: Usage: BuildProjectsDebug.bat
:: ============================================================================

setlocal

:: Logging bootstrap
if not defined LOG_CAPTURED (
    call "%~dp0internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: Delegate to implementation with Debug config
set "PARENT_BATCH=1"
call "%~dp0internal\BuildProjectsImpl.bat" Debug
set "RC=%ERRORLEVEL%"
set "PARENT_BATCH="

echo [LOG] BuildProjectsDebug.bat completed.

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
