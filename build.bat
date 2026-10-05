@echo off
setlocal enabledelayedexpansion

:: Check for compiler
where g++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    if exist "%LOCALAPPDATA%\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin\g++.exe" (
        set "PATH=%LOCALAPPDATA%\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"
    )
)

if not exist bin mkdir bin

if "%1"=="test" (
    echo [BUILD] Compiling Automated Test Suite...
    g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/ProcessManager.cpp src/VirtualFileSystem.cpp src/CapabilityManager.cpp src/AuditLogger.cpp src/AccessController.cpp tests/TestRunner.cpp -o bin\test_runner.exe
    if !ERRORLEVEL! EQU 0 (
        echo [OK] Compilation successful. Running tests...
        bin\test_runner.exe
    ) else (
        echo [!] Test compilation failed.
    )
    exit /b !ERRORLEVEL!
)

if "%1"=="clean" (
    echo [CLEAN] Cleaning build artifacts...
    if exist bin rmdir /s /q bin
    echo [OK] Clean complete.
    exit /b 0
)

echo [BUILD] Compiling Secure Capability File System...
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/ProcessManager.cpp src/VirtualFileSystem.cpp src/CapabilityManager.cpp src/AuditLogger.cpp src/AccessController.cpp src/main.cpp -o bin\secure_fs.exe

if %ERRORLEVEL% NEQ 0 (
    echo [!] Compilation failed.
    exit /b %ERRORLEVEL%
)

echo [OK] Compiled bin\secure_fs.exe successfully!

if "%1"=="run" (
    bin\secure_fs.exe
) else if "%1"=="demo" (
    bin\secure_fs.exe --demo
)
