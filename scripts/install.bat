@echo off
setlocal

rem ----------------------------------------------------------------------
echo Changing current working directory to be script's directory...
cd /d "%~dp0"

rem Specify the bakkesmod plugins folder
set "bmPluginsFolder=%APPDATA%\bakkesmod\bakkesmod\plugins"
set "dllFile=%~dp0CustomBallOnline.dll"
set "cfgFolder=%APPDATA%\bakkesmod\bakkesmod\cfg"


rem ----------------------------------------------------------------------
echo.
echo Copying .dll to bakkesmod plugins folder...

rem Check if the source folder exists before attempting to copy
if not exist "%bmPluginsFolder%" (
    echo.
    echo.
    echo Error: Destination folder does not exist: "%bmPluginsFolder%"
    echo.
    echo.
    pause
    exit /b 1
)

rem Use xcopy to copy the file to the destination folder
xcopy "%dllFile%" "%bmPluginsFolder%" /Y

rem Check the exit code of xcopy and handle any errors if needed
if errorlevel 1 (
    echo Error occurred while copying .dll file to bakkesmod plugins folder.
    echo.
    echo.
    pause
    exit /b 1
) else (
    echo Plugin .dll successfully copied to "%bmPluginsFolder%"
)


rem ----------------------------------------------------------------------
echo.
echo Adding line: "plugin load customballonline" to plugins.cfg...

cd /d %cfgFolder%

set "newLine=plugin load customballonline"

echo %newLine% | findstr /G:plugins.cfg > nul

IF %ERRORLEVEL% EQU 0 (
    echo Line already found in .cfg file
) ELSE ( 
    echo %newLine% >> "plugins.cfg"
    echo Successfully added line to plugins.cfg
)



rem success message
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