@echo off
:: ============================================================================
:: BuildSamples.bat - Interactive sample build dispatcher
:: ============================================================================
:: Prompts for build configuration and delegates to the internal implementation.
:: Supports building individual configurations or all configurations at once.
::
:: Usage: BuildSamples.bat
::   Interactive menu allows selection of: Debug, Release, RelWithDebInfo, All
::
:: Environment:
::   PARENT_BATCH  - When set, suppresses interactive prompts and pause
::   LOG_CAPTURED  - Indicates logging is already active
::   LOGFILE       - Path to current log file
:: ============================================================================

setlocal enabledelayedexpansion

:: ---------------------------------------------------------------------------
:: Logging bootstrap
:: ---------------------------------------------------------------------------
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: ---------------------------------------------------------------------------
:: Configuration selection menu
:: ---------------------------------------------------------------------------
:MENU_LOOP
echo.
echo Select build configuration:
echo   1) Debug
echo   2) Release
echo   3) RelWithDebInfo
echo   4) All
echo.
<nul set /P "=Enter choice [1-4]: " >CON
set "SEL="
set /P "SEL=" <CON

:: Default to Debug if empty
if "!SEL!"=="" set "SEL=1"

:: Map selection to configuration name
set "CONFIG="
if "!SEL!"=="1" set "CONFIG=Debug"
if "!SEL!"=="2" set "CONFIG=Release"
if "!SEL!"=="3" set "CONFIG=RelWithDebInfo"
if "!SEL!"=="4" set "CONFIG=All"

if not defined CONFIG (
    echo [ERROR] Invalid selection: '!SEL!' - Please enter 1, 2, 3, or 4.
    goto MENU_LOOP
)

echo.
echo [LOG] Selected configuration: !CONFIG!

:: Validate internal implementation exists
set "IMPL_SCRIPT=%~dp0tools\internal\BuildSamplesImpl.bat"
if not exist "!IMPL_SCRIPT!" (
    echo [ERROR] Missing: tools\internal\BuildSamplesImpl.bat
    call :FINISH 1
)

:: ---------------------------------------------------------------------------
:: Execute build(s)
:: ---------------------------------------------------------------------------
set "PARENT_BATCH=1"
set "HAS_SUCCESS=0"
set "OVERALL_RC=0"

if /I "!CONFIG!"=="All" (
    :: Track per-configuration results for launch prompt
    set "RC_Debug=1"
    set "RC_Release=1"
    set "RC_RelWithDebInfo=1"
    
    for %%C in (Debug Release RelWithDebInfo) do (
        echo.
        echo ========================================
        echo [LOG] Building: %%C
        echo ========================================
        call "!IMPL_SCRIPT!" %%C
        set "CFG_RC=!ERRORLEVEL!"
        set "RC_%%C=!CFG_RC!"
        
        if "!CFG_RC!"=="0" (
            set "HAS_SUCCESS=1"
        ) else (
            echo [ERROR] %%C build failed with code !CFG_RC!
            set "OVERALL_RC=!CFG_RC!"
        )
    )
) else (
    call "!IMPL_SCRIPT!" !CONFIG!
    set "OVERALL_RC=!ERRORLEVEL!"
    if "!OVERALL_RC!"=="0" set "HAS_SUCCESS=1"
)

set "PARENT_BATCH="

:: ---------------------------------------------------------------------------
:: Optional: Launch built executable
:: ---------------------------------------------------------------------------
if "!HAS_SUCCESS!"=="1" call :PROMPT_LAUNCH

echo.
echo [LOG] BuildSamples.bat completed.
call :FINISH !OVERALL_RC!

:: ============================================================================
:: Subroutine: Prompt to launch sample executable
:: ============================================================================
:PROMPT_LAUNCH
echo.
<nul set /P "=Launch sample executable? [y/N]: " >CON
set "LAUNCH="
set /P "LAUNCH=" <CON

:: Check for affirmative response (y or Y)
if not defined LAUNCH goto :EOF
if /I "!LAUNCH:~0,1!" NEQ "y" goto :EOF

:: Determine output directory based on configuration
set "RUN_CONFIG=!CONFIG!"
if /I "!CONFIG!"=="All" (
    :: Prefer Release, fall back to Debug if Release failed
    set "RUN_CONFIG=Release"
    if "!RC_Release!" NEQ "0" if "!RC_Debug!"=="0" set "RUN_CONFIG=Debug"
)

set "BIN_DIR=%~dp0bin\!RUN_CONFIG!"
if not exist "!BIN_DIR!" (
    echo [WARN] Output directory not found: !BIN_DIR!
    goto :EOF
)

:: Find first executable in output directory
set "TARGET_EXE="
for %%F in ("!BIN_DIR!\*.exe") do (
    if not defined TARGET_EXE set "TARGET_EXE=%%~fF"
)

if not defined TARGET_EXE (
    echo [WARN] No executables found in: !BIN_DIR!
    goto :EOF
)

:: Launch with working directory set to bin folder (for imgui.ini, etc.)
echo [LOG] Launching: !TARGET_EXE!
start "" /D "!BIN_DIR!" "!TARGET_EXE!"

:: For "All" builds, optionally launch other successful configurations
if /I "!CONFIG!"=="All" (
    for %%F in ("!TARGET_EXE!") do set "EXE_NAME=%%~nxF"
    
    for %%C in (Debug Release RelWithDebInfo) do (
        if /I "%%C" NEQ "!RUN_CONFIG!" (
            set "CHECK_RC=!RC_%%C!"
            if "!CHECK_RC!"=="0" (
                set "OTHER_EXE=%~dp0bin\%%C\!EXE_NAME!"
                if exist "!OTHER_EXE!" (
                    echo [LOG] Launching [%%C]: !OTHER_EXE!
                    start "" /D "%~dp0bin\%%C" "!OTHER_EXE!"
                )
            )
        )
    )
)
goto :EOF

:: ============================================================================
:: Subroutine: Clean exit with proper endlocal handling
:: ============================================================================
:FINISH
set "EXIT_RC=%~1"
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "EXIT_RC=%EXIT_RC%"

if defined PARENT_BATCH (
    exit /B %EXIT_RC%
)

echo.
if "%EXIT_RC%"=="0" (
    echo [SUCCESS] Build completed successfully.
) else (
    echo [ERROR] Build completed with errors.
)
echo [LOG] Logs: %LOGFILE%
pause
exit /B %EXIT_RC%

