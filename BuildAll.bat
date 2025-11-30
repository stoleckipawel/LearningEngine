@echo off
REM ---------------------------------------------------
REM Full pipeline: Clean, Check Dependencies, Generate Solution, Build Samples
REM ---------------------------------------------------

setlocal

REM --- Step 1: Clean intermediate files ---
echo [LOG] Cleaning all intermediate, build, and solution files...
call CleanIntermediate.bat
if errorlevel 1 (
    echo [ERROR] CleanWorkspace.bat failed.
    pause
    exit /B 1
)

REM --- Step 2: Check dependencies ---
echo [LOG] Checking dependencies...
call CheckDependencies.bat
if errorlevel 1 (
    echo [ERROR] CheckDependencies.bat failed.
    pause
    exit /B 1
)

REM --- Step 3: Generate solution file ---
echo [LOG] Generating Visual Studio solution...
call BuildSolution.bat
if errorlevel 1 (
    echo [ERROR] GenerateSolution.bat failed.
    pause
    exit /B 1
)

REM --- Step 4: Build all samples ---
echo [LOG] Building all sample projects...
call BuildSamples.bat
if errorlevel 1 (
    echo [ERROR] BuildSamples.bat failed.
    pause
    exit /B 1
)

echo [LOG] === Full pipeline complete! ===
pause
endlocal
REM Do not exit, leave command prompt open for user input
