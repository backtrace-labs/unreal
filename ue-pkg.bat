@echo off
setlocal enabledelayedexpansion

:: Build plugin and project scriopts for listin upload and verification
:: Deletes and recreates /verification folder
:: Hard-coded: Path to 7z, UE versions

REM Get the directory where the batch file is located
set "SCRIPT_DIR=%~dp0"
set "UE_DIR=%SCRIPT_DIR%"
set "ZIPS_DIR=%SCRIPT_DIR%verification"

REM Clear and create Zips directory
if exist "%ZIPS_DIR%" del /f/q "%ZIPS_DIR%*.zip"
if not exist "%ZIPS_DIR%" mkdir "%ZIPS_DIR%"

REM Check if 7-Zip is installed
set "ZIP_EXE="
if exist "C:\Program Files\7-Zip\7z.exe" set "ZIP_EXE=C:\Program Files\7-Zip\7z.exe"
if exist "C:\Program Files (x86)\7-Zip\7z.exe" set "ZIP_EXE=C:\Program Files (x86)\7-Zip\7z.exe"

if "%ZIP_EXE%"=="" (
    echo ERROR: 7-Zip not found.
    pause
    exit /b 1
)

echo Using 7-Zip: %ZIP_EXE%
echo.

REM Loop through UE versions 5.0 to 5.7
for /L %%v in (0,1,7) do call :ProcessVersion %%v

echo All zips created successfully in: %ZIPS_DIR%
echo.
goto :eof

:ProcessVersion
set "VERSION=5.%1"
set "UE_VERSION_DIR=%UE_DIR%\UE%VERSION%"

echo Zipping Project and Plugin for UE%VERSION%...

if not exist "%UE_VERSION_DIR%" (
    echo   WARNING: Directory %UE_VERSION_DIR% does not exist. Skipping...
    echo.
    goto :eof
)

REM Our temporary list file: (will be deleted later)
set "LISTFILE=%SCRIPT_DIR%filelist_temp.txt"

REM ==========================================
REM ===== FIRST ZIP: Project with Plugin =====
REM ==========================================
set "PROJECT_ZIP=%ZIPS_DIR%\SauceReportProjectUE%VERSION%.zip"
echo   Creating SauceReportProjectUE%VERSION%.zip...

if exist "%PROJECT_ZIP%" del "%PROJECT_ZIP%"

REM Change to UE version directory (Project Root)
pushd "%UE_VERSION_DIR%"

REM 1. Create a list file containing the EXACT relative paths we want.
REM    7-Zip will read this and preserve these paths exactly.
if exist "%LISTFILE%" del "%LISTFILE%"
echo Config>> "%LISTFILE%"
echo Source>> "%LISTFILE%"
echo Content>> "%LISTFILE%"
echo SauceSample.uproject>> "%LISTFILE%"
REM This explicitly tells 7-Zip: "Start at Plugins, go down to SauceReport..."
echo Plugins\SauceReport\Config>> "%LISTFILE%"
echo Plugins\SauceReport\Source>> "%LISTFILE%"
echo Plugins\SauceReport\SauceReport.uplugin>> "%LISTFILE%"

REM 2. Run 7-Zip using the list file (@ symbol)
"%ZIP_EXE%" a -tzip "%PROJECT_ZIP%" @"%LISTFILE%" >nul

REM Clean up list file
if exist "%LISTFILE%" del "%LISTFILE%"

popd
echo   Created: %PROJECT_ZIP%

REM ==========================================
REM ===== SECOND ZIP: Plugin Only ============
REM ==========================================
set "PLUGIN_ZIP=%ZIPS_DIR%\SauceReportPluginUE%VERSION%.zip"
echo   Creating SauceReportPluginUE%VERSION%.zip...

if exist "%PLUGIN_ZIP%" del "%PLUGIN_ZIP%"

REM Change to Plugins directory so "SauceReport" is at the root of this zip
pushd "%UE_VERSION_DIR%\Plugins"

REM Create the list of files that needs to go in the plugin' zip
if exist "%LISTFILE%" del "%LISTFILE%"
echo SauceReport\Config>> "%LISTFILE%"
echo SauceReport\Source>> "%LISTFILE%"
echo SauceReport\SauceReport.uplugin>> "%LISTFILE%"

REM Zip with 7-Zip
"%ZIP_EXE%" a -tzip "%PLUGIN_ZIP%" @"%LISTFILE%" >nul

REM Clean up (delete temp file)
if exist "%LISTFILE%" del "%LISTFILE%"

popd

echo   Created: %PLUGIN_ZIP%
echo.

goto :eof