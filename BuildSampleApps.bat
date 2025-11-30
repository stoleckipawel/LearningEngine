@echo off
REM ---------------------------------------------------
REM Build all sample projects in Debug and Release configurations
REM ---------------------------------------------------

setlocal enabledelayedexpansion
set "ARCH=x64"
set "SRC_DIR=%~dp0"
set "BUILD_DIR=!SRC_DIR!build"
set "SAMPLES_DIR=!SRC_DIR!samples"

REM --- Step 1: Clean intermediate files ---
echo [LOG] Cleaning intermediate files...
call CleanIntermediateFiles.bat CONTINUE
if errorlevel 1 (
    echo [ERROR] CleanIntermediateFiles.bat failed.
    pause
    exit /B 1
)

REM --- Step 2: Always generate solution file after cleaning ---
echo [LOG] Generating solution file using BuildSolution.bat...
call BuildSolution.bat
if errorlevel 1 (
    echo [ERROR] BuildSolution.bat failed to generate solution.
    pause
    exit /B 1
)

REM --- Step 3: Build ALL_BUILD.vcxproj to ensure all dependencies are built ---
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

REM --- Step 4: Find and build all sample projects ---
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
