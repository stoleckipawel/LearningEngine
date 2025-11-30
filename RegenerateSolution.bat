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

echo.
echo [LOG] ===============================
echo [LOG] Starting Visual Studio solution regeneration for Playground
echo [LOG] Project root: %SRC_DIR%
echo [LOG] Engine path: %ENGINE_PATH%
echo [LOG] Samples path: %SAMPLES_PATH%
echo [LOG] Build directory: %BUILD_DIR%
echo [LOG] ===============================
echo.

REM --- Set environment variables permanently for future sessions ---
echo [LOG] Setting ENGINE_PATH and SAMPLES_PATH environment variables for future sessions...
setx ENGINE_PATH "%ENGINE_PATH%"
setx SAMPLES_PATH "%SAMPLES_PATH%"

REM --- Check for CMake ---
echo [LOG] Checking for CMake in PATH...
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake and ensure it's in your PATH.
    pause
    exit /B 1
)
echo [LOG] CMake found.

echo.
REM --- Remove old build, bin, obj, and .vs folders (if exist) ---
echo [LOG] Removing old build, bin, obj, and .vs folders (if exist)...
if exist "%BUILD_DIR%" (
    echo [LOG] Deleting "%BUILD_DIR%"
    rmdir /S /Q "%BUILD_DIR%"
)
if exist "%SRC_DIR%bin" (
    echo [LOG] Deleting "%SRC_DIR%bin"
    rmdir /S /Q "%SRC_DIR%bin"
)
if exist "%SRC_DIR%obj" (
    echo [LOG] Deleting "%SRC_DIR%obj"
    rmdir /S /Q "%SRC_DIR%obj"
)
if exist "%SRC_DIR%.vs" (
    echo [LOG] Deleting "%SRC_DIR%.vs"
    rmdir /S /Q "%SRC_DIR%.vs"
)

echo.
REM --- Create build folder ---
echo [LOG] Creating build folder: "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

echo.
REM --- Generate Visual Studio solution with CMake ---
echo.
echo === Generation complete. Solution located in %BUILD_DIR% ===
echo [LOG] Generating Visual Studio solution with CMake...
cd /D "%BUILD_DIR%"

echo [LOG] Running: cmake -G "%GENERATOR%" -A %ARCH% %SRC_DIR%
cmake -G "%GENERATOR%" -A %ARCH% %SRC_DIR%
if errorlevel 1 (
    echo [ERROR] CMake generation failed.
    pause
    exit /B 1
)

echo [LOG] Generation complete. Solution located in %BUILD_DIR%
echo [LOG] You can now open the solution in Visual Studio.
pause

endlocal
exit /B 0