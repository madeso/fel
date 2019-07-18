#include "fel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

namespace fel
{
  struct File
  {
    std::string filename;
    std::string data;
    int next_index = 0;
    int col = 0;
    int line = 1;
    
    File(const std::string& f, const std::string& d) : filename(f), data(d) {}

    bool HasMore() const { return next_index < data.size(); }

    char Read()
    {
      if(next_index < data.size())
      {
        const auto read = data[next_index];
        next_index += 1;
        if(read == '\n')
        {
          line += 1;
          col = 0;
        }
        else
        {
          col += 1;
        }
        return read;
      }
      else
      {
        return 0;
      }
    }

    char Peek(int advance=1) const
    {
      const auto index = (next_index-1) + advance;
      if(index < data.size()) return data[index];
      else return 0;
    }
  };

  struct Parsed
  {
  };

  void Add(Log* log, const File& file, const std::string& message)
  {
    Add(log, file.filename, file.line, file.col, message);
  }

  Parsed Parse(File* file, Log* log)
  {
    Parsed parsed;

    while(file->HasMore())
    {
      Add(log, *file, "Char found");
      return parsed;
    }

    return parsed;
  }

  void Run(const Parsed& parsed, Log* log)
  {
  }

  void Fel::LoadAndRunString(const std::string& str, const std::string& filename, Log* log)
  {
    auto file = File{filename, str};
    auto parsed = Parse(&file, log);
    if(IsEmpty(*log)) { return; }

    Run(parsed, log);
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
  }
}

