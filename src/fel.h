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
    using FunctionCallback = std::function<void ()>;

    void SetFunction(const std::string& name, FunctionCallback callback);
    void LoadAndRunString(const std::string& str, const std::string& filename, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);

    std::map<std::string, FunctionCallback> functions;
  };
}

#endif // FEL_H
