@echo off
REM Wrapper: Build samples in Debug configuration (tools).
REM Bootstraps logging locally if needed, then calls internal implementation with PARENT_BATCH set.
setlocal
set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..\") do set "ROOT_DIR=%%~fI"

REM If logging not yet captured, delegate to centralized bootstrap helper
if not defined LOG_CAPTURED (
	call "%~dp0internal\BootstrapLog.bat" "%~f0" %*
	exit /B %ERRORLEVEL%
)

set "PARENT_BATCH=1"
call "%~dp0internal\BuildSamplesImpl.bat" Debug %1
set RC=%ERRORLEVEL%
endlocal
exit /B %RC%