@echo off
REM ---------------------------------------------------
REM CheckDependencies.bat (cleaned)
REM ---------------------------------------------------
REM Verifies required build tools are available and reports status.
REM ---------------------------------------------------

setlocal

REM Bootstrap logging via tools\internal\BootstrapLog.bat
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

REM Overall return code (0 == OK)
set "RC=0"

REM --- Check for CMake ---
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake and ensure it's in your PATH.
    set "RC=1"
) else (
    echo [SUCCESS] CMake found.
)

REM --- Check for Clang (optional) ---
where clang >nul 2>&1
if errorlevel 1 (
    echo [WARN] Clang not found. Will use MSVC toolset for generation and build.
) else (
    echo [SUCCESS] Clang found. Will use ClangCL toolset for generation and build.
)

REM --- Check for MSBuild ---Zz
where msbuild >nul 2>&1
if errorlevel 1 (
    echo [ERROR] MSBuild not found. Please install Visual Studio with the C++ workload and ensure MSBuild is in your PATH.
    set "RC=1"
) else (
    echo [SUCCESS] MSBuild found.
)

REM --- Check for clang-format (optional) ---
where clang-format >nul 2>&1
if errorlevel 1 (
    echo [WARN] clang-format not found. Install clang-format or add it to PATH if you want automatic formatting checks.
) else (
    echo [SUCCESS] clang-format found.
)

REM --- Check for clang-tidy (optional) ---
where clang-tidy >nul 2>&1
if errorlevel 1 (
    echo [WARN] clang-tidy not found. Install clang-tidy or add it to PATH to enable static analysis/format checks.
) else (
    echo [SUCCESS] clang-tidy found.
)

if %RC%==0 (
    echo [SUCCESS] All dependency checks passed.
) else (
    echo [ERROR] One or more dependency checks failed.
)

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM If called by parent, exit immediately; otherwise show status and pause so user can read logs
if defined PARENT_BATCH (
    exit /B 0
) else (
    echo.
    echo [LOG] Logs: %LOGFILE%
    pause
    exit /B 0
)
