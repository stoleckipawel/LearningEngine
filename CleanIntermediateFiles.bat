@echo off
REM ---------------------------------------------------
REM Clean all build, solution, and temporary files for Playground project
REM ---------------------------------------------------

setlocal

REM --- Set key directory variables ---
set "SRC_DIR=%~dp0"
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
for %%f in ("%SRC_DIR%*.sln" "%SRC_DIR%*.vcxproj" "%SRC_DIR%*.vcxproj.filters" "%SRC_DIR%*.vcxproj.user") do (
    if exist %%f (
        echo [CLEAN] Deleting Visual Studio file: %%f
        del /F /Q %%f
    )
)

REM --- Remove CMake cache, files, and folders ---
for %%f in ("%SRC_DIR%CMakeCache.txt" "%SRC_DIR%CMakeFiles" "%SRC_DIR%cmake_install.cmake" "%SRC_DIR%Makefile") do (
    if exist %%f (
        echo [CLEAN] Deleting CMake file: %%f
        del /F /Q %%f
    )
    if exist %%f\ (
        echo [CLEAN] Deleting CMake directory: %%f
        rmdir /S /Q %%f
    )
)

REM --- Remove Ninja intermediate files from root ---
for %%f in ("%SRC_DIR%build.ninja" "%SRC_DIR%.ninja_log" "%SRC_DIR%.ninja_deps") do (
    if exist %%f (
        echo [CLEAN] Deleting Ninja file: %%f
        del /F /Q %%f
    )
)

REM --- Remove Ninja intermediate files from build folder ---
for %%f in ("%BUILD_DIR%build.ninja" "%BUILD_DIR%.ninja_log" "%BUILD_DIR%.ninja_deps") do (
    if exist %%f (
        echo [CLEAN] Deleting Ninja file: %%f
        del /F /Q %%f
    )
)

REM --- Final status ---
echo.
echo [CLEAN] === Clean complete. All build, solution, and temporary files have been removed. ===
echo.
echo [LOG] Press any key to exit and review the output.
pause
endlocal
REM Do not exit, leave command prompt open for user input
