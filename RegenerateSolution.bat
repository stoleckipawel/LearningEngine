@echo off
REM ---------------------------------------------------
REM Regenerate Visual Studio solution for Playground
REM ---------------------------------------------------

REM === Configuration ===
setlocal
set "ARCH=x64"
set "GENERATOR=Visual Studio 17 2022"

REM --- Project root directory ---
set "SRC_DIR=%~dp0"

REM --- Dynamically set ENGINE_PATH and SAMPLES_PATH from project structure ---
set "ENGINE_PATH=%SRC_DIR%engine"
set "SAMPLES_PATH=%SRC_DIR%samples"
set "BUILD_DIR=%SRC_DIR%build"

REM --- Log environment variable values ---
echo.
echo [INFO] ENGINE_PATH is set to: %ENGINE_PATH%
echo [INFO] SAMPLES_PATH is set to: %SAMPLES_PATH%
echo.

REM --- Set environment variables permanently for future sessions ---
setx ENGINE_PATH "%ENGINE_PATH%"
setx SAMPLES_PATH "%SAMPLES_PATH%"

REM --- Check for CMake ---
echo === Checking for CMake ===
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found. Please install CMake and ensure it's in your PATH.
    pause
    exit /B 1
)

echo.
REM --- Remove old build, bin, obj, and .vs folders (if exist) ---
echo === Removing old build, bin, obj, and .vs folders (if exist) ===
if exist "%BUILD_DIR%" (
    echo Deleting "%BUILD_DIR%"
    rmdir /S /Q "%BUILD_DIR%"
)
if exist "%SRC_DIR%bin" (
    echo Deleting "%SRC_DIR%bin"
    rmdir /S /Q "%SRC_DIR%bin"
)
if exist "%SRC_DIR%obj" (
    echo Deleting "%SRC_DIR%obj"
    rmdir /S /Q "%SRC_DIR%obj"
)
if exist "%SRC_DIR%.vs" (
    echo Deleting "%SRC_DIR%.vs"
    rmdir /S /Q "%SRC_DIR%.vs"
)

echo.
REM --- Create build folder ---
echo === Creating build folder ===
mkdir "%BUILD_DIR%"

echo.
REM --- Generate Visual Studio solution with CMake ---
echo === Generating Visual Studio solution with CMake ===
cd /D "%BUILD_DIR%"

cmake -G "%GENERATOR%" -A %ARCH% %SRC_DIR%
if errorlevel 1 (
    echo ERROR: CMake generation failed.
    pause
    exit /B 1
)

echo.
echo === Generation complete. Solution located in %BUILD_DIR% ===
pause
endlocal
exit /B 0