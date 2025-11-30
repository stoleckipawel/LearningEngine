@echo off
REM ---------------------------------------------------
REM Check for required dependencies: CMake, MSBuild, Visual Studio
REM ---------------------------------------------------

setlocal

REM --- Check for CMake ---
echo [LOG] Checking for CMake in PATH...
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake and ensure it's in your PATH.
    exit /B 1
) else (
    echo [LOG] CMake found.
)

REM --- Check for Clang ---
echo [LOG] Checking for Clang in PATH...
where clang >nul 2>&1
if errorlevel 1 (
    echo [WARN] Clang not found. Will use MSVC toolset for generation and build.
) else (
    echo [LOG] Clang found. Will use ClangCL toolset for generation and build.
)

REM --- Check for MSBuild ---
echo [LOG] Checking for MSBuild in PATH...
where msbuild >nul 2>&1
if errorlevel 1 (
    echo [ERROR] MSBuild not found. Please install Visual Studio, with C++ workload and ensure MSBuild is in your PATH.
    exit /B 1
) else (
    echo [LOG] MSBuild found.
)

echo.
REM Only pause if no CONTINUE argument is provided
if /I "%1"=="CONTINUE" (
    REM Do not pause, exit immediately
) else (
    pause
)
endlocal
REM Do not exit, leave command prompt open for user input
