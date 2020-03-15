#pragma once

#include <string>
#include "root_directory.h" // This is a configuration file generated by CMake.

class FileSystem
{
public:
  static std::string getResPath(const char * path)
  {
    return getRoot() + R"(Game\res\)" + path;
  }

  static std::string getPath(const char* path)
  {
	  return getRoot() + path;
  }

private:
  static std::string const & getRoot()
  {
    static std::string root = solution_path;
    return root;
  }
};