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
echo Enter choice [1-4]: >CON
set "SEL="
set /P SEL=<CON
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
set "HAS_SUCCESS=0"
if /I "%CHOICE%"=="All" (
    set "BUILD_RC=0"
    set "RC_Debug=1"
    set "RC_Release=1"
    set "RC_RelWithDebInfo=1"
    for %%C in (Debug Release RelWithDebInfo) do (
        echo [LOG] Building configuration: %%C
        call "%~dp0tools\internal\BuildSamplesImpl.bat" %%C %2
        set "LAST_BUILD_RC=!ERRORLEVEL!"
        set "RC_%%C=!LAST_BUILD_RC!"
        if "!LAST_BUILD_RC!"=="0" set "HAS_SUCCESS=1"
        if not "!LAST_BUILD_RC!"=="0" (
            echo [ERROR] Build for %%C failed with exit code !LAST_BUILD_RC!.
            set "BUILD_RC=!LAST_BUILD_RC!"
        )
    )
) else (
    call "%~dp0tools\internal\BuildSamplesImpl.bat" !CHOICE! %2
    set "BUILD_RC=%ERRORLEVEL%"
    if "!BUILD_RC!"=="0" set "HAS_SUCCESS=1"
)
set "PARENT_BATCH="

REM Optional: launch a built sample executable (interactive)
if "!HAS_SUCCESS!"=="1" call :PROMPT_LAUNCH

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

REM ============================================================================
REM Subroutine: Prompt user to launch built sample
REM ============================================================================
:PROMPT_LAUNCH
echo.
<nul set /P "=Launch a sample executable now? [y/N]: " >CON
set "RUN_SAMPLE="
set /P "RUN_SAMPLE=" <CON
if not defined RUN_SAMPLE goto :EOF
if /I "!RUN_SAMPLE:~0,1!" NEQ "y" goto :EOF

REM Determine which config folder to use
set "RUN_CONFIG=!CHOICE!"
if /I "!CHOICE!"=="All" set "RUN_CONFIG=Release"
if /I "!CHOICE!"=="All" if "!RC_Debug!"=="0" set "RUN_CONFIG=Debug"

set "BIN_DIR=%~dp0bin\!RUN_CONFIG!"
if not exist "!BIN_DIR!" (
    echo [WARN] Output folder not found: "!BIN_DIR!"
    goto :EOF
)

REM Find the first .exe in the output folder
set "FOUND_EXE="
for %%F in ("!BIN_DIR!\*.exe") do (
    if not defined FOUND_EXE set "FOUND_EXE=%%~fF"
)

if not defined FOUND_EXE (
    echo [WARN] No executables found in "!BIN_DIR!".
    goto :EOF
)

echo [LOG] Launching: "!FOUND_EXE!"
start "" "!FOUND_EXE!"

REM If "All" was selected, also launch for other successful configs
if /I "!CHOICE!"=="All" (
    for %%C in (Debug Release RelWithDebInfo) do (
        if /I "%%C" NEQ "!RUN_CONFIG!" (
            set "CFG_RC=!RC_%%C!"
            if "!CFG_RC!"=="0" (
                for %%F in ("!FOUND_EXE!") do set "EXE_NAME=%%~nxF"
                set "OTHER_EXE=%~dp0bin\%%C\!EXE_NAME!"
                if exist "!OTHER_EXE!" (
                    echo [LOG] Launching (%%C): "!OTHER_EXE!"
                    start "" "!OTHER_EXE!"
                )
            )
        )
    )
)
goto :EOF

