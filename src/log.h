#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace fel
{
  struct Location
  {
    int line;
    int column;

    Location(int l=-1, int c=-1);
    bool operator==(const Location& rhs) const;
  };

  struct LogEntry
  {
    std::string file;
    std::string message;
    Location location;

    bool operator==(const LogEntry& rhs) const;
  };
  std::ostream& operator<<(std::ostream& o, const LogEntry& entry);

  struct Log
  {
    std::vector<LogEntry> entries;

    operator bool() const;
  };
  std::ostream& operator<<(std::ostream& o, const Log& log);

  void Add(Log* log, const std::string& file, const Location& location, const std::string& message);
  bool IsEmpty(const Log& log);
}

#endif // LOG_H

