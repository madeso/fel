#ifndef FEL_H
#define FEL_H

#include <string>

namespace fel
{
  struct Log
  {
  };

  struct Fel
  {
    void LoadAndRunString(const std::string& str, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);
  };
}

#endif // FEL_H
