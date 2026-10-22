@echo off
setlocal
set "CLI=C:\arduino-cli_1.5.1_Windows_64bit\arduino-cli.exe"
set "FQBN=arduino:avr:uno"
if "%PORT%"=="" set "PORT=COM10"
set "BAUD=9600"
set "SKETCH=%~dp0."

if /i "%~1"=="build"   goto build
if /i "%~1"=="upload"  goto upload
if /i "%~1"=="monitor" goto monitor
if /i "%~1"=="all"     goto all

echo Usage: %~nx0 ^<build^|upload^|monitor^|all^> [port]
echo   build    compile the sketch
echo   upload   upload to the board (default port COM10)
echo   monitor  open serial monitor (Ctrl+C to exit)
echo   all      build, upload, then monitor
echo Port can also be given as 2nd argument or via PORT env var.
exit /b 1

:build
"%CLI%" compile -b %FQBN% "%SKETCH%"
exit /b %errorlevel%

:upload
if not "%~2"=="" set "PORT=%~2"
"%CLI%" upload -b %FQBN% -p %PORT% "%SKETCH%"
exit /b %errorlevel%

:monitor
if not "%~2"=="" set "PORT=%~2"
"%CLI%" monitor -p %PORT% -c baudrate=%BAUD%
exit /b %errorlevel%

:all
call :build || exit /b 1
call "%~f0" upload %~2 || exit /b 1
call "%~f0" monitor %~2
exit /b %errorlevel%
