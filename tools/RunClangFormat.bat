@echo off
setlocal EnableDelayedExpansion

rem RunClangFormat.bat
rem Scans repository header and C/C++ source files, runs clang-format -style=file,
rem counts scanned and edited files and writes a log to the logs folder.

rem Find clang-format in PATH
where clang-format >nul 2>&1
if errorlevel 1 (
  where clang-format.exe >nul 2>&1
)
if errorlevel 1 (
  echo clang-format not found in PATH. Install it or add to PATH.
  exit /b 1
)

rem Compute repo root (two levels up from tools\internal)
set SCRIPT_DIR=%~dp0
pushd "%SCRIPT_DIR%" >nul
cd ..\..
set REPO_ROOT=%CD%
popd >nul

set LOG_DIR=%REPO_ROOT%\logs
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

set LOGFILE=%LOG_DIR%\LogClangFormat.txt
echo ClangFormat run on %DATE% %TIME% > "%LOGFILE%"
echo Repo root: %REPO_ROOT% >> "%LOGFILE%"
echo. >> "%LOGFILE%"

set /A scanned=0
set /A edited=0

set TEMPFILE=%TEMP%\clangfmt_tmp.txt

for %%E in (h hpp hh inl cpp cc cxx c) do (
  for /R "%REPO_ROOT%" %%F in (*.%%E) do (
    set /A scanned+=1
    set "file=%%~fF"
    clang-format -style=file "!file!" > "%TEMPFILE%" 2>> "%LOGFILE%"
    fc "%TEMPFILE%" "!file!" >nul 2>&1
    set "err=!ERRORLEVEL!"
    if "!err!"=="1" (
      copy /Y "%TEMPFILE%" "!file!" >nul
      echo Edited: !file! >> "%LOGFILE%"
      set /A edited+=1
    ) else if "!err!"=="0" (
      echo Unchanged: !file! >> "%LOGFILE%"
    ) else (
      echo Error formatting: !file! >> "%LOGFILE%"
    )
  )
)

echo. >> "%LOGFILE%"
echo Files scanned: %scanned% >> "%LOGFILE%"
echo Files edited: %edited% >> "%LOGFILE%"

echo Scanned: %scanned%  Edited: %edited%

if exist "%TEMPFILE%" del /f /q "%TEMPFILE%" >nul 2>&1

endlocal
exit /b 0
