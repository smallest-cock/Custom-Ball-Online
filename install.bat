@echo off
setlocal

rem Change the current working directory to the script's directory
cd /d "%~dp0"


rem Specify files/folders
set "bmPluginsFolder=%APPDATA%\bakkesmod\bakkesmod\plugins"
set "dllFile=%~dp0CustomBallOnline.dll"
set "cfgFolder=%APPDATA%\bakkesmod\bakkesmod\cfg"



rem ----------- Copy .dll file to bakkesmod plugins folder -----------

rem Check if the source folder exists before attempting to copy
if not exist "%bmPluginsFolder%" (
    echo Destination folder does not exist: "%bmPluginsFolder%"
    echo.
    echo Press any key to exit...
    timeout /t 10 >nul 2>&1
    exit /b 1
)

rem Use xcopy to copy the file to the destination folder
xcopy "%dllFile%" "%bmPluginsFolder%" /Y

rem Check the exit code of xcopy and handle any errors if needed
if errorlevel 1 (
    echo Error occurred while copying .dll file to bakkesmod plugins folder.
    echo.
    echo Press any key to exit...
    timeout /t 10 >nul 2>&1
    exit /b 1
) else (
    echo Plugin .dll file successfully copied to "%bmPluginsFolder%"
)




rem ----------- Add line: 'plugin load customballonline' to plugins.cfg -------------

cd /d %cfgFolder%

set "newLine=plugin load customballonline"

echo %newLine% | findstr /G:plugins.cfg > nul

IF %ERRORLEVEL% EQU 0 (
    echo Line already found in .cfg file...
) ELSE ( 
    echo %newLine% >> "plugins.cfg"
    echo Successfully added line to plugins.cfg
)


rem -------------------------- download plugin assets --------------------------

rem Define the folder for assets
set "ASSET_DIR=%APPDATA%\bakkesmod\bakkesmod\data\sslow_plugin_assets"

rem Create the folder if it doesn't exist
if not exist "%ASSET_DIR%" (
    mkdir "%ASSET_DIR%"
    echo Created folder: %ASSET_DIR%
)

rem Define image URLs and local file paths
set "URL1=https://raw.githubusercontent.com/smallest-cock/plugin-assets/main/images/settings-footer/discord.png"
set "URL2=https://raw.githubusercontent.com/smallest-cock/plugin-assets/main/images/settings-footer/github.png"
set "URL3=https://raw.githubusercontent.com/smallest-cock/plugin-assets/main/images/settings-footer/youtube.png"

set "FILE1=%ASSET_DIR%\discord.png"
set "FILE2=%ASSET_DIR%\github.png"
set "FILE3=%ASSET_DIR%\youtube.png"

rem Download the images
echo.
echo.
echo Downloading assets...
echo.
echo.

curl -o "%FILE1%" "%URL1%" || echo Failed to download discord.png
curl -o "%FILE2%" "%URL2%" || echo Failed to download github.png
curl -o "%FILE3%" "%URL3%" || echo Failed to download youtube.png



rem -------------------------- success message --------------------------
echo.
echo.
echo.
echo.
echo.
echo Setup successfully completed.
echo.
echo.
pause

endlocal