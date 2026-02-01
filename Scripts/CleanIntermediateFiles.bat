@echo off
:: ============================================================================
:: CleanIntermediateFiles.bat - Build artifact cleanup utility
:: ============================================================================
:: Removes all generated build artifacts, intermediate files, and caches:
::   - build/, bin/, obj/, .vs/ directories
::   - Visual Studio project files (*.sln, *.vcxproj, etc.)
::   - CMake cache and generated files
::   - Ninja build system artifacts
::
:: Usage: CleanIntermediateFiles.bat [CONTINUE]
::   CONTINUE - Suppress pause (used by parent scripts)
::
:: Environment:
::   PARENT_BATCH  - When set, suppresses pause on completion
::   LOG_CAPTURED  - Indicates logging is already active
::   LOGFILE       - Path to current log file
:: ============================================================================

setlocal

:: ---------------------------------------------------------------------------
:: Logging bootstrap
:: ---------------------------------------------------------------------------
if not defined LOG_CAPTURED (
    call "%~dp0Internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: ---------------------------------------------------------------------------
:: Resolve repository root directory
:: ---------------------------------------------------------------------------
set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR%"

:: Fallback if script is not at repo root
if not exist "%SRC_DIR%LICENSE.txt" if not exist "%SRC_DIR%CMakeLists.txt" (
    for %%I in ("%SCRIPT_DIR%..") do set "SRC_DIR=%%~fI\"
)

:: Define target directories
set "BUILD_DIR=%SRC_DIR%build"
set "BIN_DIR=%SRC_DIR%bin"
set "OBJ_DIR=%SRC_DIR%obj"
set "VS_DIR=%SRC_DIR%.vs"

echo.
echo ============================================================
echo   Cleaning Build Artifacts
echo ============================================================
echo.

:: ---------------------------------------------------------------------------
:: Remove directories
:: ---------------------------------------------------------------------------
if exist "%BUILD_DIR%" (
    echo [CLEAN] Removing: build\
    rmdir /S /Q "%BUILD_DIR%"
)

if exist "%BIN_DIR%" (
    echo [CLEAN] Removing: bin\
    rmdir /S /Q "%BIN_DIR%"
)

if exist "%OBJ_DIR%" (
    echo [CLEAN] Removing: obj\
    rmdir /S /Q "%OBJ_DIR%"
)

if exist "%VS_DIR%" (
    echo [CLEAN] Removing: .vs\
    rmdir /S /Q "%VS_DIR%"
)

:: ---------------------------------------------------------------------------
:: Remove Visual Studio project files from root
:: ---------------------------------------------------------------------------
echo [CLEAN] Removing VS project files from root...
del /F /Q "%SRC_DIR%*.sln" 2>nul
del /F /Q "%SRC_DIR%*.vcxproj" 2>nul
del /F /Q "%SRC_DIR%*.vcxproj.filters" 2>nul
del /F /Q "%SRC_DIR%*.vcxproj.user" 2>nul

:: ---------------------------------------------------------------------------
:: Remove CMake artifacts from root
:: ---------------------------------------------------------------------------
echo [CLEAN] Removing CMake artifacts...
del /F /Q "%SRC_DIR%CMakeCache.txt" 2>nul
del /F /Q "%SRC_DIR%cmake_install.cmake" 2>nul
del /F /Q "%SRC_DIR%Makefile" 2>nul

if exist "%SRC_DIR%CMakeFiles" (
    echo [CLEAN] Removing: CMakeFiles\
    rmdir /S /Q "%SRC_DIR%CMakeFiles"
)

:: ---------------------------------------------------------------------------
:: Remove Ninja artifacts
:: ---------------------------------------------------------------------------
echo [CLEAN] Removing Ninja artifacts...
del /F /Q "%SRC_DIR%build.ninja" 2>nul
del /F /Q "%SRC_DIR%.ninja_log" 2>nul
del /F /Q "%SRC_DIR%.ninja_deps" 2>nul
del /F /Q "%BUILD_DIR%build.ninja" 2>nul
del /F /Q "%BUILD_DIR%.ninja_log" 2>nul
del /F /Q "%BUILD_DIR%.ninja_deps" 2>nul

:: ---------------------------------------------------------------------------
:: Summary
:: ---------------------------------------------------------------------------
echo.
echo ============================================================
echo   [SUCCESS] Clean complete.
echo ============================================================
echo.

:: Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%"

if defined PARENT_BATCH (
    exit /B 0
)

echo [LOG] Logs: %LOGFILE%
pause
exit /B 0
