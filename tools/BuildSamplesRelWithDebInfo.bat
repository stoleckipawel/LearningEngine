@echo off
REM ---------------------------------------------------
REM BuildSamplesRelWithDebInfo.bat
REM ---------------------------------------------------
REM Builds all sample projects in the samples folder for RelWithDebInfo configuration.
REM Delegates logic to Tools\BuildSamples.bat.
REM ---------------------------------------------------

call "%~dp0internal\BuildSamplesImpl.bat" RelWithDebInfo %1
