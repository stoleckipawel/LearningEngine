@echo off
REM ---------------------------------------------------
REM BuildSamplesRelease.bat
REM ---------------------------------------------------
REM This script builds all sample projects in the samples folder for Release configuration.
REM Delegates logic to BuildSamples.bat.
REM ---------------------------------------------------

call "%~dp0internal\BuildSamplesImpl.bat" Release %1
