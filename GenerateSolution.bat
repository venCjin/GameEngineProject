@echo off
:: Create a Visual Studio 2017 solution
echo Generating Visual Studio 2017 solution ...
call premake\premake5.exe vs2017
echo.
PAUSE