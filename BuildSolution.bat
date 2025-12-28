@echo off
:: ============================================================================
:: BuildSolution.bat - Visual Studio solution generator
:: ============================================================================
:: Performs a clean build pipeline for generating the VS solution:
::   1. Validates required build dependencies
::   2. Cleans all intermediate and temporary files
::   3. Configures toolset (ClangCL preferred, MSVC fallback)
::   4. Generates Visual Studio solution via CMake
::
:: Usage: BuildSolution.bat [CONTINUE]
::   CONTINUE - Suppress interactive prompts (used by parent scripts)
::
:: Environment:
::   PARENT_BATCH  - When set, suppresses pause on completion
::   LOG_CAPTURED  - Indicates logging is already active
::   LOGFILE       - Path to current log file
:: ============================================================================

setlocal enabledelayedexpansion

:: ---------------------------------------------------------------------------
:: Logging bootstrap
:: ---------------------------------------------------------------------------
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

:: ---------------------------------------------------------------------------
:: Step 1: Validate build dependencies
:: ---------------------------------------------------------------------------
echo [LOG] Checking build dependencies...
set "PARENT_BATCH=1"
call "%~dp0CheckDependencies.bat" CONTINUE
if errorlevel 1 (
    set "PARENT_BATCH="
    echo [ERROR] Dependency check failed.
    call :FINISH 1
)

:: ---------------------------------------------------------------------------
:: Step 2: Clean intermediate files
:: ---------------------------------------------------------------------------
echo [LOG] Cleaning intermediate files...
call "%~dp0CleanIntermediateFiles.bat" CONTINUE
set "PARENT_BATCH="
if errorlevel 1 (
    echo [ERROR] Clean operation failed.
    call :FINISH 1
)

:: ---------------------------------------------------------------------------
:: Step 3: Configure build environment
:: ---------------------------------------------------------------------------
set "ARCH=x64"
set "GENERATOR=Visual Studio 17 2022"
set "CLANG_TOOLSET=ClangCL"
set "SRC_DIR=%~dp0"
:: Strip trailing backslash to avoid quoting issues with CMake
if "!SRC_DIR:~-1!"=="\" set "SRC_DIR=!SRC_DIR:~0,-1!"
set "BUILD_DIR=!SRC_DIR!\build"

:: Detect ClangCL availability
set "USE_CLANG=0"
where clang >nul 2>&1
if not errorlevel 1 set "USE_CLANG=1"

if "!USE_CLANG!"=="1" (
    echo [LOG] Toolset: ClangCL
) else (
    echo [LOG] Toolset: MSVC (Clang not found)
)

:: ---------------------------------------------------------------------------
:: Step 4: Extract project name from CMakeLists.txt
:: ---------------------------------------------------------------------------
set "PROJECT_NAME="
for /f "tokens=2 delims=( " %%P in ('findstr /i "project(" "!SRC_DIR!\CMakeLists.txt"') do (
    set "RAW_NAME=%%P"
)
:: Strip trailing parenthesis and whitespace
for /f "delims=) tokens=1" %%A in ("!RAW_NAME!") do set "PROJECT_NAME=%%A"
set "PROJECT_NAME=!PROJECT_NAME: =!"

echo [LOG] Project: !PROJECT_NAME!
set "SOLUTION_FILE=!BUILD_DIR!\!PROJECT_NAME!.sln"

:: ---------------------------------------------------------------------------
:: Step 5: Generate solution if not present
:: ---------------------------------------------------------------------------
set "SOLUTION_GENERATED=0"
if not exist "!SOLUTION_FILE!" (
    set "SOLUTION_GENERATED=1"
    echo [LOG] Generating solution...
    
    if not exist "!BUILD_DIR!" (
        echo [LOG] Creating build directory: !BUILD_DIR!
        mkdir "!BUILD_DIR!"
        if errorlevel 1 (
            echo [ERROR] Failed to create build directory.
            call :FINISH 1
        )
    )
    
    pushd "!BUILD_DIR!"
    
    if "!USE_CLANG!"=="1" (
        echo [LOG] CMake: -G "!GENERATOR!" -A !ARCH! -T !CLANG_TOOLSET!
        cmake -G "!GENERATOR!" -A !ARCH! -T !CLANG_TOOLSET! "!SRC_DIR!"
    ) else (
        echo [LOG] CMake: -G "!GENERATOR!" -A !ARCH!
        cmake -G "!GENERATOR!" -A !ARCH! "!SRC_DIR!"
    )
    
    set "CMAKE_RC=!ERRORLEVEL!"
    popd
    
    if "!CMAKE_RC!" NEQ "0" (
        echo [ERROR] CMake generation failed.
        call :FINISH 1
    )
    
    echo [LOG] Solution generated: !SOLUTION_FILE!
) else (
    echo [LOG] Solution exists: !SOLUTION_FILE!
)

:: ---------------------------------------------------------------------------
:: Optional: Open solution after generation
:: ---------------------------------------------------------------------------
if "!SOLUTION_GENERATED!"=="1" (
    if not defined PARENT_BATCH (
        echo.
        <nul set /P "=Open solution in Visual Studio? [y/N]: " >CON
        set "OPEN_VS="
        set /P "OPEN_VS=" <CON
        
        if /I "!OPEN_VS:~0,1!"=="y" (
            echo [LOG] Opening: !SOLUTION_FILE!
            start "" "!SOLUTION_FILE!"
        )
    )
)

echo.
echo [LOG] BuildSolution.bat completed.
call :FINISH 0

:: ============================================================================
:: Subroutine: Clean exit with proper endlocal handling
:: ============================================================================
:FINISH
set "EXIT_RC=%~1"
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "EXIT_RC=%EXIT_RC%"

if defined PARENT_BATCH (
    exit /B %EXIT_RC%
)

echo.
if "%EXIT_RC%"=="0" (
    echo [SUCCESS] Solution generation completed.
) else (
    echo [ERROR] Solution generation failed.
)
echo [LOG] Logs: %LOGFILE%
pause
exit /B %EXIT_RC%


