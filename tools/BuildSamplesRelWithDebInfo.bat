@echo off
REM Wrapper: Build samples in RelWithDebInfo configuration (tools).
REM Bootstraps logging locally if needed, then calls internal implementation with PARENT_BATCH set.
setlocal
set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..\") do set "ROOT_DIR=%%~fI"

if not defined LOG_CAPTURED (
	call "%~dp0internal\BootstrapLog.bat" "%~f0" %*
	exit /B %ERRORLEVEL%
)

set "PARENT_BATCH=1"
call "%~dp0internal\BuildSamplesImpl.bat" RelWithDebInfo %1
set RC=%ERRORLEVEL%

echo [LOG] BuildSamplesRelWithDebInfo.bat completed.

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM If called by parent, exit immediately; otherwise show status and pause so user can read logs
if defined PARENT_BATCH (
    exit /B 0
) else (
    echo.
    echo [LOG] Logs: %LOGFILE%
    pause
    exit /B 0
)
