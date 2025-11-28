@echo off
REM ---------------------------------------------------
REM Clean all build, solution, and temporary files for ToyRenderer
REM ---------------------------------------------------

setlocal
set "SRC_DIR=%~dp0"
set "BUILD_DIR=%SRC_DIR%build"
set "BIN_DIR=%SRC_DIR%bin"
set "OBJ_DIR=%SRC_DIR%obj"
set "VS_DIR=%SRC_DIR%.vs"

REM Remove build directory
if exist "%BUILD_DIR%" (
    echo Deleting "%BUILD_DIR%"
    rmdir /S /Q "%BUILD_DIR%"
)

REM Remove bin directory
if exist "%BIN_DIR%" (
    echo Deleting "%BIN_DIR%"
    rmdir /S /Q "%BIN_DIR%"
)

REM Remove obj directory
if exist "%OBJ_DIR%" (
    echo Deleting "%OBJ_DIR%"
    rmdir /S /Q "%OBJ_DIR%"
)

REM Remove .vs directory
if exist "%VS_DIR%" (
    echo Deleting "%VS_DIR%"
    rmdir /S /Q "%VS_DIR%"
)

REM Remove Visual Studio solution files
for %%f in ("%SRC_DIR%*.sln" "%SRC_DIR%*.vcxproj" "%SRC_DIR%*.vcxproj.filters" "%SRC_DIR%*.vcxproj.user") do (
    if exist %%f (
        echo Deleting %%f
        del /F /Q %%f
    )
)

REM Remove CMake cache and files
for %%f in ("%SRC_DIR%CMakeCache.txt" "%SRC_DIR%CMakeFiles" "%SRC_DIR%cmake_install.cmake" "%SRC_DIR%Makefile") do (
    if exist %%f (
        echo Deleting %%f
        del /F /Q %%f
    )
    if exist %%f\ (
        echo Deleting directory %%f
        rmdir /S /Q %%f
    )
)

echo.
echo === Clean complete. All build, solution, and temporary files have been removed. ===
pause
endlocal
exit /B 0
