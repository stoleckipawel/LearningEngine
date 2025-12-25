@echo off
REM ---------------------------------------------------
REM CleanIntermediateFiles.bat
REM ---------------------------------------------------
REM This script cleans all build, solution, and temporary files for the Playground project:
REM   1. Removes build, bin, obj, and .vs directories
REM   2. Deletes Visual Studio solution and project files (*.sln, *.vcxproj, etc.)
REM   3. Deletes CMake cache, files, and folders
REM   4. Deletes Ninja intermediate files from root and build folder
REM ---------------------------------------------------

setlocal

REM Bootstrap logging via tools\internal\BootstrapLog.bat
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

REM --- Set key directory variables ---
set "SCRIPT_DIR=%~dp0"
REM Determine project root: prefer the script directory if it looks like the repo root,
REM otherwise fall back to the parent directory (scripts may live in a subfolder).
set "SRC_DIR=%SCRIPT_DIR%"
if not exist "%SRC_DIR%LICENSE.txt" if not exist "%SRC_DIR%CMakeLists.txt" (
    for %%I in ("%SCRIPT_DIR%..") do set "SRC_DIR=%%~fI\"
)
set "BUILD_DIR=%SRC_DIR%build"
set "BIN_DIR=%SRC_DIR%bin"
set "OBJ_DIR=%SRC_DIR%obj"
set "VS_DIR=%SRC_DIR%.vs"

REM --- Remove build directory ---
if exist "%BUILD_DIR%" (
    echo [CLEAN] Deleting build directory: "%BUILD_DIR%"
    rmdir /S /Q "%BUILD_DIR%"
)

REM --- Remove bin directory ---
if exist "%BIN_DIR%" (
    echo [CLEAN] Deleting bin directory: "%BIN_DIR%"
    rmdir /S /Q "%BIN_DIR%"
)

REM --- Remove obj directory ---
if exist "%OBJ_DIR%" (
    echo [CLEAN] Deleting obj directory: "%OBJ_DIR%"
    rmdir /S /Q "%OBJ_DIR%"
)

REM --- Remove .vs (Visual Studio workspace) directory ---
if exist "%VS_DIR%" (
    echo [CLEAN] Deleting .vs directory: "%VS_DIR%"
    rmdir /S /Q "%VS_DIR%"
)

REM --- Remove Visual Studio solution and project files ---
echo [CLEAN] Deleting Visual Studio solution and project files
del /F /Q "%SRC_DIR%*.sln" "%SRC_DIR%*.vcxproj" "%SRC_DIR%*.vcxproj.filters" "%SRC_DIR%*.vcxproj.user" >nul 2>&1

REM --- Remove CMake cache, files, and folders ---
echo [CLEAN] Deleting CMake files and directories
del /F /Q "%SRC_DIR%CMakeCache.txt" "%SRC_DIR%cmake_install.cmake" "%SRC_DIR%Makefile" >nul 2>&1
if exist "%SRC_DIR%CMakeFiles" (
    echo [CLEAN] Deleting CMake directory: "%SRC_DIR%CMakeFiles"
    rmdir /S /Q "%SRC_DIR%CMakeFiles"
)

REM --- Remove Ninja intermediate files from root ---
echo [CLEAN] Deleting Ninja intermediate files in repo root
del /F /Q "%SRC_DIR%build.ninja" "%SRC_DIR%.ninja_log" "%SRC_DIR%.ninja_deps" >nul 2>&1

REM --- Remove Ninja intermediate files from build folder ---
echo [CLEAN] Deleting Ninja intermediate files in build folder
del /F /Q "%BUILD_DIR%build.ninja" "%BUILD_DIR%.ninja_log" "%BUILD_DIR%.ninja_deps" >nul 2>&1

REM --- Final status ---
echo.
echo [CLEAN] === Clean complete. All build, solution, and temporary files have been removed. ===
echo.

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM If called by parent, exit immediately; otherwise show status and pause so user can read logs
if defined PARENT_BATCH (
    exit /B 0
) else (
    echo.
    echo [SUCCESS] CleanIntermediateFiles completed.
    echo [INFO] Logs: %LOGFILE%
    pause
    exit /B 0
)
