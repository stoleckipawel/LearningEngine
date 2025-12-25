@echo off
REM ---------------------------------------------------
REM CheckDependencies.bat (cleaned)
REM ---------------------------------------------------
REM Verifies required build tools are available and reports status.
REM ---------------------------------------------------

setlocal

REM Bootstrap logging via centralized helper (if not already capturing)
if not defined LOG_CAPTURED (
    call "%~dp0tools\internal\BootstrapLog.bat" "%~f0" %*
    exit /B %ERRORLEVEL%
)

REM Overall return code (0 == OK)
set "RC=0"

REM --- Check for CMake ---
echo [LOG] Checking for CMake in PATH...
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake and ensure it's in your PATH.
    set "RC=1"
) else (
    echo [SUCCESS] CMake found.
)

REM --- Check for Clang (optional) ---
echo [LOG] Checking for Clang in PATH...
where clang >nul 2>&1
if errorlevel 1 (
    echo [WARN] Clang not found. Will use MSVC toolset for generation and build.
) else (
    echo [SUCCESS] Clang found. Will use ClangCL toolset for generation and build.
)

REM --- Check for MSBuild ---Zz
echo [LOG] Checking for MSBuild in PATH...
where msbuild >nul 2>&1
if errorlevel 1 (
    echo [ERROR] MSBuild not found. Please install Visual Studio with the C++ workload and ensure MSBuild is in your PATH.
    set "RC=1"
) else (
    echo [SUCCESS] MSBuild found.
)

REM Preserve LOGFILE across endlocal
set "_TMP_LOGFILE=%LOGFILE%"
endlocal & set "LOGFILE=%_TMP_LOGFILE%" & set "_TMP_LOGFILE="

REM If invoked as a child of another batch, return immediately with the RC.
if defined PARENT_BATCH (
    exit /B %RC%
)

echo.
if %RC%==0 (
    echo [SUCCESS] CheckDependencies Succeeded.
) else (
    echo [ERROR] CheckDependencies Failed (exit %RC%).
)
echo [INFO] Logs: %LOGFILE%
pause
exit /B %RC%
