#include "log.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

namespace fel
{
  bool LogEntry::operator==(const LogEntry& rhs) const
  {
    return file == rhs.file
      && message == rhs.message
      && line == rhs.line
      && column == rhs.column;
  }

  std::ostream& operator<<(std::ostream& o, const LogEntry& entry)
  {
    o << entry.file << ":" << entry.line << ":" << entry.column << ": " << entry.message;
    return o;
  }

  Log::operator bool() const
  {
    return IsEmpty(*this);
  }

  std::ostream& operator<<(std::ostream& o, const Log& log)
  {
    for(const auto& e: log.entries)
    {
      o << e.file << ":" << e.line << ":" << e.column << ": " << e.message << "\n";
    }
  }

  void Add(Log* log, const std::string& file, int line, int col, const std::string& message)
  {
    fel::LogEntry entry;
    entry.file = file;
    entry.message = message;
    entry.line = line;
    entry.column = col;
    log->entries.push_back(entry);
  }

  bool IsEmpty(const Log& log)
  {
    return log.entries.empty();
  }
}

