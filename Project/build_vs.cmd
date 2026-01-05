@echo off
setlocal

set "ROOT_DIR=%~dp0.."
set "BUILD_DIR=%~dp0VS"
set "GENERATOR=Visual Studio 17 2022"

if "%~1"=="" (
    set "PLATFORM=x64"
) else (
    set "PLATFORM=%~1"
)

if "%~2"=="" (
    set "CONFIG=Release"
) else (
    set "CONFIG=%~2"
)

if "%~3"=="" (
set "VCPKG_ROOT=%ROOT_DIR%\Project\third_party\vcpkg"
) else (
    set "VCPKG_ROOT=%~3"
)

set "VS_ARCH=%PLATFORM%"
if /I "%VS_ARCH%"=="Win32" set "VS_ARCH=x86"

if not "%VSINSTALL%"=="" (
    set "VSDEVCMD=%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
)

if not defined VSDEVCMD (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%VSWHERE%" (
        for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
        if defined VSINSTALL set "VSDEVCMD=%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
    )
)

if not defined VSDEVCMD (
    if exist "%ProgramFiles%\Microsoft Visual Studio" (
        for /d %%v in ("%ProgramFiles%\Microsoft Visual Studio\*") do (
            for /d %%e in ("%%v\*") do (
                if exist "%%e\Common7\Tools\VsDevCmd.bat" set "VSDEVCMD=%%e\Common7\Tools\VsDevCmd.bat"
            )
        )
    )
)

if not defined VSDEVCMD (
    if exist "%ProgramFiles(x86)%\Microsoft Visual Studio" (
        for /d %%v in ("%ProgramFiles(x86)%\Microsoft Visual Studio\*") do (
            for /d %%e in ("%%v\*") do (
                if exist "%%e\Common7\Tools\VsDevCmd.bat" set "VSDEVCMD=%%e\Common7\Tools\VsDevCmd.bat"
            )
        )
    )
)

if not defined VSDEVCMD (
    echo Visual Studio developer environment not found.
    exit /b 1
)

call "%VSDEVCMD%" -arch=%VS_ARCH% -host_arch=x64
if errorlevel 1 exit /b 1

set "VCPKG_TRIPLET=%PLATFORM%"
if /I "%VCPKG_TRIPLET%"=="Win32" set "VCPKG_TRIPLET=x86"
if /I "%VCPKG_TRIPLET%"=="x64" set "VCPKG_TRIPLET=x64"
set "VCPKG_TRIPLET=%VCPKG_TRIPLET%-windows"

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    if not exist "%VCPKG_ROOT%" mkdir "%VCPKG_ROOT%"
    if not exist "%VCPKG_ROOT%\.git" (
        git clone https://github.com/microsoft/vcpkg "%VCPKG_ROOT%"
        if errorlevel 1 exit /b 1
    )
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat"
    if errorlevel 1 exit /b 1
)

set "VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"

if exist "%VCPKG_ROOT%\vcpkg.exe" (
    "%VCPKG_ROOT%\vcpkg.exe" install --triplet %VCPKG_TRIPLET%
    if errorlevel 1 exit /b 1
)

if exist "%BUILD_DIR%\CMakeCache.txt" (
    findstr /C:"CMAKE_TOOLCHAIN_FILE:FILEPATH=%VCPKG_TOOLCHAIN%" "%BUILD_DIR%\CMakeCache.txt" >nul
    if errorlevel 1 (
        del /f /q "%BUILD_DIR%\CMakeCache.txt" >nul 2>&1
        if exist "%BUILD_DIR%\CMakeFiles" rmdir /s /q "%BUILD_DIR%\CMakeFiles"
    )
)

if exist "%VCPKG_TOOLCHAIN%" (
    cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%" -G "%GENERATOR%" -A "%PLATFORM%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" -DVCPKG_TARGET_TRIPLET=%VCPKG_TRIPLET%
) else (
    cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%" -G "%GENERATOR%" -A "%PLATFORM%"
)
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" --config "%CONFIG%"
if errorlevel 1 exit /b 1

echo Built %CONFIG% for %PLATFORM% in %BUILD_DIR%
