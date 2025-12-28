@echo off
:: ============================================================================
:: BuildSamplesImpl.bat - Internal sample build implementation
:: ============================================================================
:: Builds all sample projects under samples/ for a specified configuration.
:: Invoked by configuration wrappers or the root BuildSamples.bat dispatcher.
::
:: Usage: BuildSamplesImpl.bat <Configuration> [extra_args]
::   Configuration: Debug | Release | RelWithDebInfo
::
:: Environment:
::   PARENT_BATCH  - When set, suppresses pause on completion
::   LOG_CAPTURED  - Indicates logging is already active
::   LOGFILE       - Path to current log file
:: ============================================================================

setlocal enabledelayedexpansion

:: ---------------------------------------------------------------------------
:: Path configuration
:: ---------------------------------------------------------------------------
set "ARCH=x64"
set "SCRIPT_DIR=%~dp0"
:: Script resides in tools\internal; repository root is two levels up
for %%I in ("%SCRIPT_DIR%..\..\") do set "SRC_DIR=%%~fI"
set "BUILD_DIR=!SRC_DIR!build"
set "SAMPLES_DIR=!SRC_DIR!samples"

:: ---------------------------------------------------------------------------
:: Logging bootstrap (if not already captured by parent)
:: ---------------------------------------------------------------------------
if not defined LOG_CAPTURED (
    call "%SCRIPT_DIR%BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: ---------------------------------------------------------------------------
:: Parse configuration argument
:: ---------------------------------------------------------------------------
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

:: MSBuild properties for RelWithDebInfo: preserve full debug symbols
set "MSBUILD_EXTRA="
if /I "%CONFIG%"=="RelWithDebInfo" (
    echo [LOG] RelWithDebInfo: Enabling full debug information.
    set "MSBUILD_EXTRA=/p:DebugSymbols=true /p:DebugType=full /p:GenerateDebugInformation=true /p:Optimize=true /p:LinkIncremental=false"
)

:: ---------------------------------------------------------------------------
:: Ensure solution exists (generate if missing)
:: ---------------------------------------------------------------------------
set "PROJECT_NAME="
for /f "tokens=2 delims=( " %%P in ('findstr /i "project(" "!SRC_DIR!CMakeLists.txt"') do (
    set "RAW_PROJECT_NAME=%%P"
)
for /f "delims=) tokens=1" %%A in ("!RAW_PROJECT_NAME!") do set "PROJECT_NAME=%%A"
set "PROJECT_NAME=!PROJECT_NAME: =!"
set "SOLUTION_FILE=!BUILD_DIR!\!PROJECT_NAME!.sln"

if not exist "!SOLUTION_FILE!" (
    echo [LOG] Solution not found. Invoking BuildSolution.bat...
    set "PARENT_BATCH=1"
    call "!SRC_DIR!BuildSolution.bat" CONTINUE
    set "PARENT_BATCH="
    if errorlevel 1 (
        echo [ERROR] Solution generation failed.
        call :FINISH 1
    )
)

:: ---------------------------------------------------------------------------
:: Build all sample projects
:: ---------------------------------------------------------------------------
set "BUILD_FAILED=0"
for /D %%S in ("!SAMPLES_DIR!\*") do (
    set "SAMPLE_NAME=%%~nxS"
    set "SAMPLE_VCXPROJ=!BUILD_DIR!\samples\!SAMPLE_NAME!\!SAMPLE_NAME!.vcxproj"
    
    if exist "!SAMPLE_VCXPROJ!" (
        echo [LOG] Building: !SAMPLE_NAME! [!CONFIG!]
        msbuild "!SAMPLE_VCXPROJ!" /p:Configuration=!CONFIG! /p:Platform=!ARCH! !MSBUILD_EXTRA! /nologo /v:minimal
        if errorlevel 1 (
            echo [ERROR] Build failed: !SAMPLE_NAME! [!CONFIG!]
            set "BUILD_FAILED=1"
        ) else (
            echo [SUCCESS] !SAMPLE_NAME! [!CONFIG!] - Output: bin\!CONFIG!
        )
    ) else (
        echo [WARN] No project found: !SAMPLE_NAME! - Skipping.
    )
)

if "!BUILD_FAILED!"=="1" (
    call :FINISH 1
)

echo [LOG] All samples built successfully for !CONFIG!.
call :FINISH 0

:: ---------------------------------------------------------------------------
:: Subroutine: Clean exit with proper endlocal handling
:: ---------------------------------------------------------------------------
:FINISH
set "EXIT_RC=%~1"
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "EXIT_RC=%EXIT_RC%"

if defined PARENT_BATCH (
    exit /B %EXIT_RC%
)

echo.
if "%EXIT_RC%"=="0" (
    echo [SUCCESS] Sample build completed.
) else (
    echo [ERROR] Sample build failed.
)
echo [LOG] Logs: %LOGFILE%
pause
exit /B %EXIT_RC%
