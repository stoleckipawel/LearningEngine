@echo off
REM ---------------------------------------------------
REM BuildSamplesImpl.bat (internal implementation)
REM ---------------------------------------------------
REM Internal implementation invoked by tools wrappers or root BuildSamples.bat.
REM Respects PARENT_BATCH to avoid pausing when called by a parent.
REM ---------------------------------------------------

setlocal enabledelayedexpansion
set "ARCH=x64"
set "SCRIPT_DIR=%~dp0"
REM Script is under tools\internal; repo root is two levels up.
for %%I in ("%SCRIPT_DIR%..\..\") do set "SRC_DIR=%%~fI"
set "BUILD_DIR=!SRC_DIR!build"
set "SAMPLES_DIR=!SRC_DIR!samples"

REM --- Logging bootstrap via centralized helper (if not already captured) ---
if not defined LOG_CAPTURED (
    call "%~dp0BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

REM --- Step 0: Parse configuration argument ---
set "CONFIG=%1"
if /I "%CONFIG%"=="" set "CONFIG=Debug"

REM --- Step 0.5: Prepare MSBuild extra properties for specific configs ---
set "MSBUILD_EXTRA="
if /I "%CONFIG%"=="RelWithDebInfo" (
    echo [LOG] RelWithDebInfo detected: enabling full debug info flags for MSBuild.
    set "MSBUILD_EXTRA=/p:DebugSymbols=true /p:DebugType=full /p:GenerateDebugInformation=true /p:Optimize=true /p:LinkIncremental=false"
)

REM --- Step 1: Check if solution file exists, generate if missing ---
set "PROJECT_NAME="
for /f "tokens=2 delims=( " %%P in ('findstr /i "project(" "!SRC_DIR!CMakeLists.txt"') do (
    set "RAW_PROJECT_NAME=%%P"
)
for /f "delims=) tokens=1" %%A in ("%RAW_PROJECT_NAME%") do set "PROJECT_NAME=%%A"
set "PROJECT_NAME=%PROJECT_NAME: =%"
set "SOLUTION_FILE=!BUILD_DIR!\%PROJECT_NAME%.sln"
if not exist "!SOLUTION_FILE!" (
    echo [LOG] Solution file not found. Generating with BuildSolution.bat...
    set "PARENT_BATCH=1"
    call "!SRC_DIR!BuildSolution.bat" CONTINUE
    set "PARENT_BATCH="
    if errorlevel 1 (
        echo [ERROR] BuildSolution.bat failed to generate solution.
        if not defined PARENT_BATCH (
            echo [ERROR] See logs: %LOGFILE%
            pause
        )
        exit /B 1
    )
)

REM --- Step 2: Find and build all sample projects for given configuration ---
for /D %%S in ("!SAMPLES_DIR!\*") do (
    set "SAMPLE_NAME=%%~nxS"
    set "SAMPLE_VCXPROJ=!BUILD_DIR!\samples\!SAMPLE_NAME!\!SAMPLE_NAME!.vcxproj"
    if exist "!SAMPLE_VCXPROJ!" (
        echo [LOG] Building sample project: !SAMPLE_NAME! %CONFIG%...
        msbuild "!SAMPLE_VCXPROJ!" /p:Configuration=%CONFIG% /p:Platform=!ARCH! !MSBUILD_EXTRA!
        if errorlevel 1 (
            echo [ERROR] Build failed for !SAMPLE_NAME! %CONFIG%.
            if not defined PARENT_BATCH (
                echo [ERROR] See logs: %LOGFILE%
                pause
            )
            endlocal
            exit /B 1
        )
        echo [SUCCESS] !SAMPLE_NAME! built successfully for %CONFIG%. Executable should be in bin\%CONFIG%.
    ) else (
        echo [WARN] No vcxproj found for sample !SAMPLE_NAME! in build/samples/!SAMPLE_NAME!. Skipping.
    )
)

echo [LOG] All sample %CONFIG% builds complete. Check bin\%CONFIG% for executables.

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

if defined PARENT_BATCH (
    exit /B 0
) else (
    echo.
    echo [SUCCESS] Samples build completed.
    echo [INFO] Logs: %LOGFILE%
    pause
    exit /B 0
)
