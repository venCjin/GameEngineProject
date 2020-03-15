@echo off

:: Create a solution_path variable
echo Generating Engine/src/root_directory.h ...
set a=const char * solution_path = R"(
set c=)";
set line=%a%%__CD__%%c%
echo %line%>Engine/src/root_directory.h
echo Generated line: %line%
echo.
echo Generated file:
type Engine\src\root_directory.h
echo.


:: Create a Visual Studio 2019 solution
:: If created before generated files,
:: won't include them into solution
echo Generating Visual Studio 2019 solution ...
call premake\premake5.exe vs2019
echo.

PAUSE