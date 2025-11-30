@echo off
REM ---------------------------------------------------
REM Build all sample projects in Debug and Release configurations
REM ---------------------------------------------------

setlocal enabledelayedexpansion
set "ARCH=x64"
set "GENERATOR=Visual Studio 17 2022"
set "CLANG_TOOLSET=ClangCL"
set "SRC_DIR=%~dp0"
set "BUILD_DIR=!SRC_DIR!build"
set "SAMPLES_DIR=!SRC_DIR!samples"

REM --- Read project name from CMakeLists.txt ---
for /f "tokens=2 delims=( )" %%P in ('findstr /i "project(" "!SRC_DIR!CMakeLists.txt"') do set "PROJECT_NAME=%%P"
set "SOLUTION_FILE=!BUILD_DIR!\!PROJECT_NAME!.sln"

REM --- Build ALL_BUILD.vcxproj to ensure all dependencies are built ---
if exist "!BUILD_DIR!\ALL_BUILD.vcxproj" (
    for %%C in (Debug Release) do (
        echo [LOG] Building ALL_BUILD.vcxproj for configuration %%C...
        msbuild "!BUILD_DIR!\ALL_BUILD.vcxproj" /p:Configuration=%%C /p:Platform=!ARCH!
        if errorlevel 1 (
            echo [ERROR] ALL_BUILD.vcxproj build failed for configuration %%C.
            echo [LOG] Press any key to exit and review the output.
            pause
            endlocal
            exit /B 1
        )
        echo [SUCCESS] ALL_BUILD.vcxproj built for %%C.
    )
)

REM --- Find and build all sample projects ---
for /D %%S in ("!SAMPLES_DIR!\*") do (
    set "SAMPLE_NAME=%%~nxS"
    if exist "!BUILD_DIR!\!SAMPLE_NAME!.vcxproj" (
        for %%C in (Debug Release) do (
            echo [LOG] Building sample project: !SAMPLE_NAME! (%%C)...
            msbuild "!BUILD_DIR!\!SAMPLE_NAME!.vcxproj" /p:Configuration=%%C /p:Platform=!ARCH!
            if errorlevel 1 (
                echo [ERROR] Build failed for !SAMPLE_NAME! (%%C).
                echo [LOG] Press any key to exit and review the output.
                pause
                endlocal
                exit /B 1
            )
            echo [SUCCESS] !SAMPLE_NAME! built successfully for %%C. Executable should be in bin/%%C.
        )
    ) else (
        echo [WARN] No vcxproj found for sample !SAMPLE_NAME! in build folder. Skipping.
    )
)

echo [LOG] All sample builds complete. Check bin/Debug and bin/Release for executables.
echo.
echo [LOG] Press any key to exit and review the output.
pause
endlocal
REM Do not exit, leave command prompt open for user input
