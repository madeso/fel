#ifndef FEL_H
#define FEL_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace fel
{
  struct LogEntry
  {
    std::string file;
    std::string message;
    int line = 0;
    int column = 0;

    bool operator==(const LogEntry& rhs) const;
  };

  struct Log
  {
    std::vector<LogEntry> entries;
  };

  struct Fel
  {
    using Callback = std::function<void(const std::string&)>;

    std::map<std::string, Callback> functions;

    void SetFunction(const std::string& name, Callback callback);
    void LoadAndRunString(const std::string& str, const std::string& filename, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);
  };
}

#endif // FEL_H
