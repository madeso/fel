#ifndef FEL_H
#define FEL_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "log.h"

namespace fel
{
  struct State
  {
    int arguments = 0;
    std::vector<int> stack;

    int GetStack(int index) const;
    int GetArg(int index) const;
  };

  struct Fel
  {
    using FunctionCallback = std::function<void (State*)>;

    void SetFunction(const std::string& name, FunctionCallback callback);
    void LoadAndRunString(const std::string& str, const std::string& filename, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);

    std::map<std::string, FunctionCallback> functions;
  };
}

#endif // FEL_H
