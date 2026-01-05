@echo off
setlocal

set "ROOT_DIR=%~dp0.."
set "VCPKG_ROOT=%ROOT_DIR%\Project\third_party\vcpkg"

if "%~1"=="" (
    set "TRIPLET=x64-windows"
) else (
    set "TRIPLET=%~1"
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    if not exist "%VCPKG_ROOT%" mkdir "%VCPKG_ROOT%"
    if not exist "%VCPKG_ROOT%\.git" (
        git clone https://github.com/microsoft/vcpkg "%VCPKG_ROOT%"
        if errorlevel 1 exit /b 1
    )
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat"
    if errorlevel 1 exit /b 1
)

call "%VCPKG_ROOT%\vcpkg.exe" install qt5-base --triplet %TRIPLET%
if errorlevel 1 exit /b 1

echo Vcpkg ready in %VCPKG_ROOT% with qt5-base (%TRIPLET%).
