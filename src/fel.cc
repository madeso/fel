#include "fel.h"

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


  void Log::AddLog(const std::string& file, int line, int col, const std::string& message)
  {
    fel::LogEntry entry;
    entry.file = file;
    entry.message = message;
    entry.line = line;
    entry.column = col;
    entries.push_back(entry);
  }

  void Fel::LoadAndRunString(const std::string& str, const std::string& filename, Log* log)
  {
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
  }
}

