@echo off
REM ---------------------------------------------------
REM BuildSamplesRelWithDebInfo.bat
REM ---------------------------------------------------
REM Builds all sample projects in the samples folder for RelWithDebInfo configuration.
REM Delegates logic to Tools\BuildSamples.bat.
REM ---------------------------------------------------

call Tools\BuildSamples.bat RelWithDebInfo %1
