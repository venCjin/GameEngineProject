@echo off

:: Create a Visual Studio 2019 solution
echo Generating Visual Studio 2019 solution ...
call premake\premake5.exe vs2019
echo.

:: Create a solution_path variable
echo Generating Engine/src/path_config.h ...
set a=const std::string solution_path = R"(
set c=)";
set line=%a%%__CD__%%c%
echo %line%>Engine/src/path_config.h
echo Generated line: %line%

set a=const std::string resources_path = R"(
set c=)";
set line=%a%%__CD__%Game\res\%c%
echo %line% >> Engine/src/path_config.h
echo Generated line: %line%

echo Generated file:
type Engine\src\path_config.h
echo.

PAUSE