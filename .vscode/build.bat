@echo off
rem バッチファイルの場所(.vscode/)から1階層上がプロジェクトルート
set PROJDIR=%~dp0..

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" > nul 2>&1
set CMAKE="C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

%CMAKE% -S "%PROJDIR%" -B "%PROJDIR%\build" -G Ninja
if errorlevel 1 exit /b 1
%CMAKE% --build "%PROJDIR%\build"
