#ifndef FEL_H
#define FEL_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "log.h"

namespace fel
{
  struct Fel
  {
    void LoadAndRunString(const std::string& str, const std::string& filename, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);
  };
}

#endif // FEL_H
