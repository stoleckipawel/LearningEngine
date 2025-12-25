@echo off
REM BootstrapLog.bat
REM Usage: call "...\BootstrapLog.bat" "<caller_full_path>" [args...]
REM If LOG_CAPTURED is not set, this will create logs folder, generate a timestamped
REM log file, re-run the caller under redirection, copy latest to LogTools.txt, and
REM return the exit code. If LOG_CAPTURED is already set, it returns immediately.

setlocal

if defined LOG_CAPTURED (
    endlocal
    exit /B 0
)

if "%~1"=="" (
    echo [ERROR] BootstrapLog requires caller path as first argument.
    endlocal
    exit /B 1
)

set "CALLER=%~1"
shift

REM Determine repo root relative to the CALLER's location (prefer caller over helper)
for %%F in ("%CALLER%") do set "CALLER_DIR=%%~dpF"
REM Prefer locating the repo root by looking for LICENSE.txt in the caller directory
if exist "%CALLER_DIR%LICENSE.txt" (
    set "ROOT_DIR=%CALLER_DIR%"
)

if defined ROOT_DIR (
    if not "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR%\"
) else (
    set "ROOT_DIR=%~dp0"
    if not "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR%\"
)
REM Ensure ROOT_DIR ends with a backslash
if not "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR%\"

if not exist "%ROOT_DIR%logs" mkdir "%ROOT_DIR%logs" >nul 2>&1

set "ts=%DATE%_%TIME%"
set "ts=%ts::=-%"
set "ts=%ts:/=-%"
set "ts=%ts:,=-%"
set "ts=%ts:.=-%"
set "ts=%ts: =_%"
set "LOGFILE=%ROOT_DIR%logs\logTools_%ts%.txt"

powershell -NoProfile -ExecutionPolicy Bypass -Command "$env:LOG_CAPTURED='1'; $env:LOGFILE='%LOGFILE%'; & cmd /c '\"%CALLER%\" %*' 2>&1 | Tee-Object -FilePath '%LOGFILE%'; exit $LastExitCode"
set "RC=%ERRORLEVEL%"

copy /Y "%LOGFILE%" "%ROOT_DIR%logs\logTools.txt" >nul 2>&1

endlocal
exit /B %RC%
