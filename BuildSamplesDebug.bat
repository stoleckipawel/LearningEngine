@echo off
echo [LOG] All sample Debug builds complete. Check bin/Debug for executables.
echo.
REM ---------------------------------------------------
REM BuildSamplesDebug.bat
REM ---------------------------------------------------
REM This script builds all sample projects in the samples folder for Debug configuration.
REM Delegates logic to BuildSamples.bat.
REM ---------------------------------------------------

call Tools\BuildSamples.bat Debug %1