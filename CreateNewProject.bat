@echo off
:: ============================================================================
:: CreateNewProject.bat - Sparkle Project Generator
:: ============================================================================
:: Creates a new Sparkle project from the TemplateProject.
::
:: Usage: CreateNewProject.bat [ProjectName]
::   If ProjectName is omitted, prompts interactively.
::
:: The script will:
::   1. Copy the template from projects/TemplateProject
::   2. Replace __PROJECT_NAME__ placeholder with actual project name
::   3. Create .sparkle-project marker file
::   4. Prompt to regenerate the VS solution
:: ============================================================================

setlocal enabledelayedexpansion

:: ---------------------------------------------------------------------------
:: Resolve paths
:: ---------------------------------------------------------------------------
set "SCRIPT_DIR=%~dp0"
set "ROOT_DIR=%SCRIPT_DIR%"
set "TEMPLATE_DIR=%ROOT_DIR%projects\TemplateProject"
set "PROJECTS_DIR=%ROOT_DIR%projects"

:: ---------------------------------------------------------------------------
:: Validate template exists
:: ---------------------------------------------------------------------------
if not exist "%TEMPLATE_DIR%" (
    echo [ERROR] Template directory not found: %TEMPLATE_DIR%
    echo         Please ensure the projects/TemplateProject folder exists.
    goto :ERROR_EXIT
)

:: ---------------------------------------------------------------------------
:: Get project name from argument or prompt
:: ---------------------------------------------------------------------------
set "PROJECT_NAME=%~1"
if "%PROJECT_NAME%"=="" (
    echo.
    echo ============================================================
    echo   Sparkle Project Generator
    echo ============================================================
    echo.
    set /p "PROJECT_NAME=Enter project name (e.g. MyGame): "
)

:: Validate project name is not empty
if "%PROJECT_NAME%"=="" (
    echo [ERROR] Project name cannot be empty.
    goto :ERROR_EXIT
)

:: ---------------------------------------------------------------------------
:: Validate project name (alphanumeric + underscore only)
:: ---------------------------------------------------------------------------
echo %PROJECT_NAME%| findstr /r "^[A-Za-z_][A-Za-z0-9_]*$" >nul
if errorlevel 1 (
    echo [ERROR] Invalid project name: %PROJECT_NAME%
    echo         Project name must start with a letter or underscore,
    echo         and contain only letters, numbers, and underscores.
    goto :ERROR_EXIT
)

:: ---------------------------------------------------------------------------
:: Check if project already exists
:: ---------------------------------------------------------------------------
set "PROJECT_DIR=%PROJECTS_DIR%\%PROJECT_NAME%"
if exist "%PROJECT_DIR%" (
    echo [ERROR] Project already exists: %PROJECT_DIR%
    echo         Choose a different name or delete the existing project.
    goto :ERROR_EXIT
)

:: ---------------------------------------------------------------------------
:: Create project directory structure
:: ---------------------------------------------------------------------------
echo.
echo [LOG] Creating project: %PROJECT_NAME%
echo [LOG] Destination: %PROJECT_DIR%
echo.

:: Create projects folder if it doesn't exist
if not exist "%PROJECTS_DIR%" (
    echo [LOG] Creating projects directory...
    mkdir "%PROJECTS_DIR%"
)

:: ---------------------------------------------------------------------------
:: Copy template to new project
:: ---------------------------------------------------------------------------
echo [LOG] Copying template files...
xcopy /E /I /Q "%TEMPLATE_DIR%" "%PROJECT_DIR%" >nul
if errorlevel 1 (
    echo [ERROR] Failed to copy template files.
    goto :ERROR_EXIT
)

:: ---------------------------------------------------------------------------
:: Replace __PROJECT_NAME__ placeholder in all files
:: ---------------------------------------------------------------------------
echo [LOG] Configuring project files...

:: Process CMakeLists.txt
call :REPLACE_PLACEHOLDER "%PROJECT_DIR%\CMakeLists.txt"

:: Process main.cpp
call :REPLACE_PLACEHOLDER "%PROJECT_DIR%\src\main.cpp"

:: ---------------------------------------------------------------------------
:: Success message
:: ---------------------------------------------------------------------------
echo.
echo ============================================================
echo   [SUCCESS] Project Created
echo ============================================================
echo.
echo   Name:     %PROJECT_NAME%
echo   Location: %PROJECT_DIR%
echo.
echo   Next steps:
echo   1. Run BuildSolution.bat to regenerate VS solution
echo   2. Open build\Sparkle.sln in Visual Studio
echo   3. Set %PROJECT_NAME% as startup project
echo.
echo ============================================================

:: Ask to rebuild solution
echo.
echo ============================================================
echo   Regenerate VS Solution?
echo ============================================================
echo.
echo   Y^) Yes - Regenerate now
echo   N^) No  - Skip for now
echo.
echo ============================================================

:REBUILD_PROMPT
set "REBUILD="
set /p "REBUILD=Enter choice [Y/N]: "
if /i "%REBUILD%"=="Y" goto :DO_REBUILD
if /i "%REBUILD%"=="N" goto :SUCCESS_EXIT
if "%REBUILD%"=="" goto :SUCCESS_EXIT
echo [ERROR] Please enter Y or N.
goto :REBUILD_PROMPT

:DO_REBUILD
echo.
echo [LOG] Regenerating Visual Studio solution...
call "%ROOT_DIR%BuildSolution.bat"

:SUCCESS_EXIT
echo.
if not defined PARENT_BATCH pause
exit /B 0

:ERROR_EXIT
echo.
if not defined PARENT_BATCH pause
exit /B 1

:: ---------------------------------------------------------------------------
:: Subroutine: Replace __PROJECT_NAME__ in a file using PowerShell
:: ---------------------------------------------------------------------------
:REPLACE_PLACEHOLDER
set "TARGET_FILE=%~1"
if not exist "%TARGET_FILE%" (
    echo [WARN] File not found for substitution: %TARGET_FILE%
    exit /B 0
)

:: Use PowerShell for reliable text replacement
powershell -NoProfile -Command ^
    "$content = Get-Content -Path '%TARGET_FILE%' -Raw; " ^
    "$content = $content -replace '__PROJECT_NAME__', '%PROJECT_NAME%'; " ^
    "Set-Content -Path '%TARGET_FILE%' -Value $content -NoNewline"

if errorlevel 1 (
    echo [WARN] Failed to process: %TARGET_FILE%
)
exit /B 0
