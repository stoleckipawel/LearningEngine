@echo off
:: ============================================================================
:: RunClangFormat.bat - Code formatting utility
:: ============================================================================
:: Runs clang-format on all source files under engine/ and samples/.
:: Displays progress, tracks modifications, and generates a summary log.
::
:: Supported extensions: .cpp, .h, .hlsl, .hlsli
:: Output log: logs/LogClangFormat.txt
::
:: Usage: RunClangFormat.bat
:: ============================================================================

setlocal enabledelayedexpansion

:: ---------------------------------------------------------------------------
:: Resolve repository root
:: ---------------------------------------------------------------------------
pushd "%~dp0.." >nul
set "ROOT_DIR=%CD%\"
popd >nul

:: ---------------------------------------------------------------------------
:: Prepare log directory and file
:: ---------------------------------------------------------------------------
set "LOG_DIR=%ROOT_DIR%logs"
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"
set "LOGFILE=%LOG_DIR%\LogClangFormat.txt"

echo [LOG] ClangFormat started: %DATE% %TIME% > "%LOGFILE%"

:: ---------------------------------------------------------------------------
:: Count total files to process (for progress display)
:: ---------------------------------------------------------------------------
echo [LOG] Scanning for source files...

set /A TOTAL=0
for %%E in (cpp h hlsl hlsli) do (
    for /R "%ROOT_DIR%engine" %%F in (*.%%E) do (
        echo %%~fF | findstr /I /C:"\logs\" >nul || set /A TOTAL+=1
    )
    for /R "%ROOT_DIR%samples" %%F in (*.%%E) do (
        echo %%~fF | findstr /I /C:"\logs\" >nul || set /A TOTAL+=1
    )
)

if %TOTAL%==0 (
    echo [WARN] No source files found.
    echo [LOG] No files found. >> "%LOGFILE%"
    goto :SUMMARY
)

echo [LOG] Found %TOTAL% files to process.
echo.

:: ---------------------------------------------------------------------------
:: Process each file
:: ---------------------------------------------------------------------------
set /A IDX=0
set /A MODIFIED=0

for %%E in (cpp h hlsl hlsli) do (
    call :PROCESS_DIR "%ROOT_DIR%engine" %%E
    call :PROCESS_DIR "%ROOT_DIR%samples" %%E
)

goto :SUMMARY

:: ---------------------------------------------------------------------------
:: Subroutine: Process all files of given extension in directory tree
:: ---------------------------------------------------------------------------
:PROCESS_DIR
set "SCAN_DIR=%~1"
set "EXT=%~2"

for /R "%SCAN_DIR%" %%F in (*.%EXT%) do (
    :: Skip files in logs directory
    echo %%~fF | findstr /I /C:"\logs\" >nul
    if errorlevel 1 (
        set /A IDX+=1
        set "FILE=%%~fF"
        
        :: Display progress
        echo [!IDX!/!TOTAL!] !FILE!
        echo [SCAN] !IDX!/!TOTAL! - !FILE! >> "%LOGFILE%"
        
        :: Compute hash before formatting
        set "HASH_BEFORE="
        for /f "skip=1 tokens=1" %%H in ('certutil -hashfile "%%~fF" MD5 2^>nul') do (
            if not defined HASH_BEFORE set "HASH_BEFORE=%%H"
        )
        
        :: Run clang-format in-place
        clang-format -style=file -i "%%~fF" 2>>"%LOGFILE%"
        if errorlevel 1 (
            echo [ERROR] clang-format failed: %%~fF >> "%LOGFILE%"
        )
        
        :: Compute hash after formatting
        set "HASH_AFTER="
        for /f "skip=1 tokens=1" %%H in ('certutil -hashfile "%%~fF" MD5 2^>nul') do (
            if not defined HASH_AFTER set "HASH_AFTER=%%H"
        )
        
        :: Check if file was modified
        if not "!HASH_BEFORE!"=="!HASH_AFTER!" (
            set /A MODIFIED+=1
            echo [MODIFIED] !FILE! >> "%LOGFILE%"
            echo   ^> Modified
        )
    )
)
goto :EOF

:: ---------------------------------------------------------------------------
:: Summary
:: ---------------------------------------------------------------------------
:SUMMARY
echo.
echo ========================================
echo  Scanned:  !IDX! files
echo  Modified: !MODIFIED! files
echo ========================================
echo.

echo [LOG] Summary: Scanned=!IDX! Modified=!MODIFIED! >> "%LOGFILE%"
echo [LOG] ClangFormat finished: %DATE% %TIME% >> "%LOGFILE%"

endlocal
pause
