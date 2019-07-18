#ifndef LOG_H
#define LOG_H

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
  std::ostream& operator<<(std::ostream& o, const LogEntry& entry);

  struct Log
  {
    std::vector<LogEntry> entries;

    operator bool() const;
  };
  std::ostream& operator<<(std::ostream& o, const Log& log);

  void Add(Log* log, const std::string& file, int line, int col, const std::string& message);
  bool IsEmpty(const Log& log);
}

#endif // LOG_H

