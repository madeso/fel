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
      && location == rhs.location;
  }

  std::ostream& operator<<(std::ostream& o, const LogEntry& entry)
  {
    o << entry.file << ":" << entry.location.line << ":" << entry.location.column << ": " << entry.message;
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
      o << e.file << ":" << e.location.line << ":" << e.location.column << ": " << e.message << "\n";
    }
    o << log.entries.size() << " error(s) detected\n";
  }

  void Add(Log* log, const std::string& file, const Location& location, const std::string& message)
  {
    fel::LogEntry entry;
    entry.file = file;
    entry.message = message;
    entry.location = location;
    log->entries.push_back(entry);
  }

  bool IsEmpty(const Log& log)
  {
    return log.entries.empty();
  }
}

