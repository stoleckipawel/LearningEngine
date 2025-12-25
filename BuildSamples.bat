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
echo  4^) All
set /P "SEL=Enter choice [1-4]: "
if "!SEL!"=="" set "SEL=1"
if "!SEL!"=="1" set "CHOICE=Debug"
if "!SEL!"=="2" set "CHOICE=Release"
if "!SEL!"=="3" set "CHOICE=RelWithDebInfo"
if "!SEL!"=="4" set "CHOICE=All"
if "!CHOICE!"=="" (
    echo Invalid selection '!SEL!'. Please choose 1, 2, 3 or 4.
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
if /I "%CHOICE%"=="All" (
    set "BUILD_RC=0"
    for %%C in (Debug Release RelWithDebInfo) do (
        echo [LOG] Building configuration: %%C
        call "%~dp0tools\internal\BuildSamplesImpl.bat" %%C %2
        set "LAST_BUILD_RC=!ERRORLEVEL!"
        if not "!LAST_BUILD_RC!"=="0" (
            echo [ERROR] Build for %%C failed with exit code !LAST_BUILD_RC!.
            set "BUILD_RC=!LAST_BUILD_RC!"
        )
    )
) else (
    call "%~dp0tools\internal\BuildSamplesImpl.bat" !CHOICE! %2
    set "BUILD_RC=%ERRORLEVEL%"
)
set "PARENT_BATCH="

echo [LOG] BuildSamples.bat completed.

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM If called by parent, exit immediately; otherwise show status and pause so user can read logs
if defined PARENT_BATCH (
    exit /B 0
) else (
    echo.
    echo [SUCCESS] BuildSamples completed.
    echo [LOG] Logs: %LOGFILE%
    pause
    exit /B 0
)

