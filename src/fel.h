#ifndef FEL_H
#define FEL_H

#include <string>
#include <vector>

namespace fel
{
  struct LogEntry
  {
    std::string file;
    std::string message;
    int line = 0;
    int column = 0;
  };

  struct Log
  {
    std::vector<LogEntry> entries;
  };

  struct Fel
  {
    void LoadAndRunString(const std::string& str, const std::string& filename, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);
  };
}

#endif // FEL_H
