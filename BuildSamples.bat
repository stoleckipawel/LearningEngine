@echo off
REM Root BuildSamples.bat - prompt user for configuration and delegate to tools\internal\BuildSamplesImpl.bat

setlocal enabledelayedexpansion

REM Bootstrap logging via tools\internal\BootstrapLog.bat
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

REM Always prompt the user for the configuration (ignore positional args)
:PROMPT_LOOP
echo Select configuration to build:
echo  1^) Debug
echo  2^) Release
echo  3^) RelWithDebInfo
set /P "SEL=Enter choice [1-3]: "
if "!SEL!"=="" set "SEL=1"
if "!SEL!"=="1" set "CHOICE=Debug"
if "!SEL!"=="2" set "CHOICE=Release"
if "!SEL!"=="3" set "CHOICE=RelWithDebInfo"
if "!CHOICE!"=="" (
    echo Invalid selection '!SEL!'. Please choose 1, 2 or 3.
    goto PROMPT_LOOP
)

REM Normalize common textual arguments
if /I "%CHOICE%"=="debug" set "CHOICE=Debug"
if /I "%CHOICE%"=="release" set "CHOICE=Release"
if /I "%CHOICE%"=="relwithdebinfo" set "CHOICE=RelWithDebInfo"

echo Building configuration: %CHOICE%

if not exist "%~dp0tools\internal\BuildSamplesImpl.bat" (
    echo [ERROR] Internal build implementation not found: tools\internal\BuildSamplesImpl.bat
    exit /B 1
)

REM Mark that we're the parent so children don't pause at the end
set "PARENT_BATCH=1"
    call "%~dp0tools\internal\BuildSamplesImpl.bat" !CHOICE! %2
    set RC=%ERRORLEVEL%
    set "PARENT_BATCH="

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM Final top-level status: show result and where log is located
if %RC%==0 (
    echo [SUCCESS] BuildSamples completed.
    echo [INFO] Logs: %LOGFILE%
) else (
    echo [ERROR] BuildSamples failed (exit %RC%).
    echo [INFO] Logs: %LOGFILE%
)
pause
exit /B %RC%
