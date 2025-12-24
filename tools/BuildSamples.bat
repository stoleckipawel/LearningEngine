@echo off
REM ---------------------------------------------------
REM BuildSamples.bat
REM ---------------------------------------------------
REM This script builds all sample projects in the samples folder for a given configuration (Debug/Release):
REM   1. Checks if solution file exists, generates if missing
REM   2. Finds and builds all sample projects (*.vcxproj) in samples for the given configuration
REM   3. Logs each build step and result
REM   4. At the end, pauses for user review unless called with CONTINUE argument
REM ---------------------------------------------------

setlocal enabledelayedexpansion
set "ARCH=x64"
set "SCRIPT_DIR=%~dp0"
REM If the script lives under the Tools folder, use the parent directory as the project root.
for %%I in ("%SCRIPT_DIR%..") do set "SRC_DIR=%%~fI\"
set "BUILD_DIR=!SRC_DIR!build"
set "SAMPLES_DIR=!SRC_DIR!samples"

REM --- Step 0: Parse configuration argument ---
set "CONFIG=%1"
if /I "!CONFIG!"=="" set "CONFIG=Debug"

REM --- Step 0.5: Prepare MSBuild extra properties for specific configs ---
set "MSBUILD_EXTRA="
if /I "!CONFIG!"=="RelWithDebInfo" (
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
    call BuildSolution.bat CONTINUE
    if errorlevel 1 (
        echo [ERROR] BuildSolution.bat failed to generate solution.
        pause
        exit /B 1
    )
)

REM --- Step 2: Find and build all sample projects for given configuration ---
for /D %%S in ("!SAMPLES_DIR!\*") do (
    set "SAMPLE_NAME=%%~nxS"
    set "SAMPLE_VCXPROJ=!BUILD_DIR!\samples\!SAMPLE_NAME!\!SAMPLE_NAME!.vcxproj"
    if exist "!SAMPLE_VCXPROJ!" (
        echo [LOG] Building sample project: !SAMPLE_NAME! !CONFIG!...
        msbuild "!SAMPLE_VCXPROJ!" /p:Configuration=!CONFIG! /p:Platform=!ARCH! !MSBUILD_EXTRA!
        if errorlevel 1 (
            echo [ERROR] Build failed for !SAMPLE_NAME! !CONFIG!.
            echo [LOG] Press any key to exit and review the output.
            pause
            endlocal
            exit /B 1
        )
        echo [SUCCESS] !SAMPLE_NAME! built successfully for !CONFIG!. Executable should be in bin/!CONFIG!.
    ) else (
        echo [WARN] No vcxproj found for sample !SAMPLE_NAME! in build/samples/!SAMPLE_NAME!. Skipping.
    )
)

echo [LOG] All sample !CONFIG! builds complete. Check bin/!CONFIG! for executables.
echo.
REM Only pause if no CONTINUE argument is provided
if /I "%2"=="CONTINUE" (
    REM Do not pause, exit immediately
) else (
    echo [LOG] Press any key to exit and review the output.
    pause
)
endlocal
REM Do not exit, leave command prompt open for user input
