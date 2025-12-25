@echo off
REM RunClangFormat.bat
REM Runs clang-format on all .cpp .h .hlsl .hlsli files under the repo root,
REM shows progress, counts scanned and modified files, and writes a log to Logs/LogClangFormat.txt

setlocal enabledelayedexpansion

set "ROOT_DIR=%~dp0"
set "LOG_DIR=%ROOT_DIR%Logs"
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"
set "LOGFILE=%LOG_DIR%\LogClangFormat.txt"

echo [LOG] ClangFormat run started: %DATE% %TIME%>"%LOGFILE%"

set /A total=0
for %%e in (cpp h hlsl hlsli) do (
    for /R "%ROOT_DIR%" %%F in (*.%%e) do (
        echo %%~fF | findstr /I /C:"\\Logs\\" >nul
        if errorlevel 1 (
            set /A total+=1
        )
    )
)

if %total%==0 (
    echo No files found to format.
    echo [LOG] No files found.>>"%LOGFILE%"
    exit /B 0
)

set /A idx=0
set /A modified=0

for %%e in (cpp h hlsl hlsli) do (
    for /R "%ROOT_DIR%" %%F in (*.%%e) do (
        echo %%~fF | findstr /I /C:"\\Logs\\" >nul
        if errorlevel 1 (
            set /A idx+=1
            set "file=%%~fF"
            echo Progress: !idx!/!total! - !file!
            echo [SCAN] !idx!/!total! - !file!>>"%LOGFILE%"

            set "orighash="
            for /f "skip=1 tokens=1" %%H in ('certutil -hashfile "%%~fF" MD5 2^>nul') do if not defined orighash set "orighash=%%H"

            clang-format -style=file -i "%%~fF" 2>>"%LOGFILE%"
            if errorlevel 1 (
                echo [ERROR] clang-format failed for: %%~fF>>"%LOGFILE%"
            )

            set "newhash="
            for /f "skip=1 tokens=1" %%H in ('certutil -hashfile "%%~fF" MD5 2^>nul') do if not defined newhash set "newhash=%%H"

            if not "!orighash!"=="!newhash!" (
                set /A modified+=1
                echo [MODIFIED] !file!>>"%LOGFILE%"
                echo Modified.
            ) else (
                echo Unchanged.
            )
        )
    )
)

echo.
echo Scanned files: !idx!
echo Modified files: !modified!
echo.
echo [LOG] Summary: Scanned=!idx! Modified=!modified! >> "%LOGFILE%"
echo [LOG] ClangFormat run finished: %DATE% %TIME% >> "%LOGFILE%"

endlocal
