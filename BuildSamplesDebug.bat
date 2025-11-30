@echo off
REM ---------------------------------------------------
REM BuildSamplesDebug.bat
REM ---------------------------------------------------
REM This script builds only sample projects in the samples folder for Debug configuration:
REM   1. Finds and builds all sample projects (*.vcxproj) in samples for Debug
REM   2. Logs each build step and result
REM   3. At the end, pauses for user review unless called with CONTINUE argument
REM ---------------------------------------------------

setlocal enabledelayedexpansion
set "ARCH=x64"
set "SRC_DIR=%~dp0"
set "BUILD_DIR=!SRC_DIR!build"
set "SAMPLES_DIR=!SRC_DIR!samples"


REM --- Step 0: Check if solution file exists, generate if missing ---
set "PROJECT_NAME="
for /f "tokens=2 delims=( " %%P in ('findstr /i "project(" "%SRC_DIR%CMakeLists.txt"') do (
    set "RAW_PROJECT_NAME=%%P"
)
for /f "delims=) tokens=1" %%A in ("%RAW_PROJECT_NAME%") do set "PROJECT_NAME=%%A"
set "PROJECT_NAME=%PROJECT_NAME: =%"
set "SOLUTION_FILE=!BUILD_DIR!\%PROJECT_NAME%.sln"
if not exist "!SOLUTION_FILE!" (
    echo [LOG] Solution file not found. Generating with BuildSolution.bat...
    call BuildSolution.bat CONTINUE
    if errorlevel 1 (
        echo [ERROR] BuildSolution.bat failed to generate solution.
        pause
        exit /B 1
    )
)

REM --- Step 1: Find and build all sample projects for Debug ---
for /D %%S in ("!SAMPLES_DIR!\*") do (
    set "SAMPLE_NAME=%%~nxS"
    set "SAMPLE_VCXPROJ=!BUILD_DIR!\samples\!SAMPLE_NAME!\!SAMPLE_NAME!.vcxproj"
    if exist "!SAMPLE_VCXPROJ!" (
        echo [LOG] Building sample project: !SAMPLE_NAME! Debug...
        msbuild "!SAMPLE_VCXPROJ!" /p:Configuration=Debug /p:Platform=!ARCH!
        if errorlevel 1 (
            echo [ERROR] Build failed for !SAMPLE_NAME! Debug.
            echo [LOG] Press any key to exit and review the output.
            pause
            endlocal
            exit /B 1
        )
        echo [SUCCESS] !SAMPLE_NAME! built successfully for Debug. Executable should be in bin/Debug.
    ) else (
        echo [WARN] No vcxproj found for sample !SAMPLE_NAME! in build/samples/!SAMPLE_NAME!. Skipping.
    )
)

echo [LOG] All sample Debug builds complete. Check bin/Debug for executables.
echo.
REM Only pause if no CONTINUE argument is provided
if /I "%1"=="CONTINUE" (
    REM Do not pause, exit immediately
) else (
    echo [LOG] Press any key to exit and review the output.
    pause
)
endlocal
REM Do not exit, leave command prompt open for user input