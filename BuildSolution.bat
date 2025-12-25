@echo off
REM ---------------------------------------------------
REM BuildSolution.bat
REM ---------------------------------------------------
REM This script performs a clean build pipeline for the Visual Studio solution:
REM   0. Checks for required build dependencies using CheckDependencies.bat
REM   1. Cleans all intermediate, build, and temporary files using CleanIntermediateFiles.bat
REM   2. Configures build variables and toolset selection (ClangCL or MSVC)
REM   3. Extracts the project name from CMakeLists.txt
REM   4. Generates the Visual Studio solution using CMake
REM   5. Logs all steps and errors for review
REM ---------------------------------------------------

setlocal enabledelayedexpansion

REM Bootstrap logging via tools\internal\BootstrapLog.bat
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

REM Step 0: Check dependencies before proceeding
echo [LOG] Checking build dependencies...
set "PARENT_BATCH=1"
call "%~dp0CheckDependencies.bat" CONTINUE
if errorlevel 1 (
    set "PARENT_BATCH="
    echo [ERROR] Dependency check failed. Please review missing dependencies above.
    if defined LOGFILE echo Logs: %LOGFILE%
    exit /B 1
)

REM Step 1: Clean all intermediate files
echo [LOG] Cleaning intermediate files...
call "%~dp0CleanIntermediateFiles.bat" CONTINUE
set "PARENT_BATCH="
if errorlevel 1 (
    echo [ERROR] CleanIntermediateFiles.bat failed.
    if defined LOGFILE echo Logs: %LOGFILE%
    exit /B 1
)

REM Step 2: Configuration
set "ARCH=x64"
set "GENERATOR=Visual Studio 17 2022"
set "CLANG_TOOLSET=ClangCL"
set "SRC_DIR=%~dp0"
set "BUILD_DIR=!SRC_DIR!build"

REM Step 3: Toolset selection
set "USE_CLANG=0"
where clang >nul 2>&1
if not errorlevel 1 set "USE_CLANG=1"
if "!USE_CLANG!"=="1" (
    echo [LOG] ClangCL toolset will be used for generation and build.
) else (
    echo [LOG] MSVC toolset will be used for generation and build.
)

REM Step 4: Extract project name from CMakeLists.txt
for /f "tokens=2 delims=( " %%P in ('findstr /i "project(" "!SRC_DIR!CMakeLists.txt"') do (
    set "RAW_PROJECT_NAME=%%P"
)
REM Clean up project name (remove trailing ')' and whitespace)
for /f "delims=) tokens=1" %%A in ("%RAW_PROJECT_NAME%") do set "PROJECT_NAME=%%A"
set "PROJECT_NAME=%PROJECT_NAME: =%"
echo [LOG] Project name found in CMakeLists.txt: %PROJECT_NAME%
set "SOLUTION_FILE=!BUILD_DIR!\%PROJECT_NAME%.sln"
echo [LOG] Solution file path set to: !SOLUTION_FILE!

REM Step 5: Generate solution if missing
if not exist "!SOLUTION_FILE!" (
    echo [LOG] Solution file not found. Starting generation process...
    if not exist "!BUILD_DIR!" (
        echo [LOG] Build folder does not exist. Creating: "!BUILD_DIR!"
        mkdir "!BUILD_DIR!"
        if errorlevel 1 (
            echo [ERROR] Failed to create build folder: "!BUILD_DIR!"
            pause
            exit /B 1
        )
    ) else (
        echo [LOG] Build folder already exists: "!BUILD_DIR!"
    )
    cd /D "!BUILD_DIR!"
    if "!USE_CLANG!"=="1" (
        echo [LOG] Running CMake with ClangCL toolset...
        echo [LOG] Command: cmake -G "!GENERATOR!" -A !ARCH! -T !CLANG_TOOLSET! !SRC_DIR!
        cmake -G "!GENERATOR!" -A !ARCH! -T !CLANG_TOOLSET! !SRC_DIR!
    ) else (
        echo [LOG] Running CMake with MSVC toolset...
        echo [LOG] Command: cmake -G "!GENERATOR!" -A !ARCH! !SRC_DIR!
        cmake -G "!GENERATOR!" -A !ARCH! !SRC_DIR!
    )
    if errorlevel 1 (
        echo [ERROR] CMake generation failed. Check above output for details.
        pause
        exit /B 1
    )
    cd /D "!SRC_DIR!"
    echo [LOG] Solution successfully generated: "!SOLUTION_FILE!"
) else (
    echo [LOG] Solution file already exists: "!SOLUTION_FILE!"
)

echo [LOG] BuildSolution.bat completed.

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM If called by parent, exit immediately; otherwise show status and pause so user can read logs
if defined PARENT_BATCH (
    exit /B 0
) else (
    echo.
    echo [LOG] Logs: %LOGFILE%
    pause
    exit /B 0
)


