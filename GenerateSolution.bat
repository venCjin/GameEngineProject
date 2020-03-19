@echo off
:: Create a Visual Studio 2019 solution
echo Generating Visual Studio 2019 solution ...
call premake\premake5.exe vs2019
echo.
PAUSE