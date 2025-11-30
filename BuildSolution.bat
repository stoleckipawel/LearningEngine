@echo off
REM ---------------------------------------------------
REM Generate Visual Studio solution from CMakeLists.txt
REM ---------------------------------------------------

setlocal enabledelayedexpansion

REM --- Configuration ---
set "ARCH=x64"
set "GENERATOR=Visual Studio 17 2022"
set "CLANG_TOOLSET=ClangCL"
set "SRC_DIR=%~dp0"
set "BUILD_DIR=!SRC_DIR!build"

REM --- Toolset selection ---
set "USE_CLANG=0"
where clang >nul 2>&1
if not errorlevel 1 set "USE_CLANG=1"
if "!USE_CLANG!"=="1" (
    echo [LOG] ClangCL toolset will be used for generation and build.
) else (
    echo [LOG] MSVC toolset will be used for generation and build.
)

REM --- Extract project name from CMakeLists.txt ---
for /f "tokens=2 delims=( " %%P in ('findstr /i "project(" "!SRC_DIR!CMakeLists.txt"') do (
    set "RAW_PROJECT_NAME=%%P"
)
REM --- Clean up project name (remove trailing ')' and whitespace) ---
for /f "delims=) tokens=1" %%A in ("%RAW_PROJECT_NAME%") do set "PROJECT_NAME=%%A"
set "PROJECT_NAME=%PROJECT_NAME: =%"
echo [LOG] Project name found in CMakeLists.txt: %PROJECT_NAME%
set "SOLUTION_FILE=!BUILD_DIR!\%PROJECT_NAME%.sln"
echo [LOG] Solution file path set to: !SOLUTION_FILE!

REM --- Generate solution if missing ---
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
pause
endlocal
REM Do not exit, leave command prompt open for user input

