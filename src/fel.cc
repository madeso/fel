#include "fel.h"

#include <string>
#include <cassert>
#include <sstream>

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

  bool IsNum(char c)
  {
    return c >= '0' && c <= '9';
  }

  bool IsFirstIdent(char c)
  {
    if(c >= 'a' && c <= 'z') return true;
    if(c >= 'a' && c <= 'Z') return true;
    switch(c)
    {
      case '_': return true;
      default: return false;
    }
  }

  bool IsSpace(char c)
  {
    switch(c)
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        return true;
      default:
        return false;
    }
  }

  std::string ParseIdent(File* file)
  {
    if(!IsFirstIdent(file->Peek()))
    {
      return "";
    }
    std::ostringstream ss;
    ss << file->Read();
    while(file->HasMore())
    {
      const auto p = file->Peek();
      if(IsFirstIdent(p) || IsNum(p))
      {
        ss << file->Read();
      }
      else
      {
        break;
      }
    }
    return ss.str();
  }

  void SkipSpaces(File* file)
  {
    while(file->HasMore() && IsSpace(file->Peek()))
    {
      file->Read(); // skip
    }
  }

  struct Parsed
  {
  };

  void Add(Log* log, const File& file, const std::string& message)
  {
    Add(log, file.filename, file.line, file.col, message);
  }

  std::string CharToString(char c)
  {
    switch(c)
    {
      case ' ': return "<space>";
      case '\n': return "<newline>";
      case '\t': return "<tab>";
      case '\r': return "<return>";
      case 0: return "<null>";
      default:
              std::stringstream ss;
              ss << c;
              return ss.str();
    }
  }

  Parsed Parse(File* file, Log* log)
  {
    Parsed parsed;

#define EXPECT(cc) \
    do\
    {\
      const auto ec = file->Read();\
      if(ec != cc)\
      {\
        Add(log, *file, "Error: Expected " #cc " but found " + CharToString(ec));\
        return parsed;\
      }\
    } while(false)

    SkipSpaces(file);
    while(file->HasMore())
    {
      const auto first = file->Peek();
      if(IsFirstIdent(first))
      {
        const auto ident = ParseIdent(file);
        if(ident == "")
        {
          Add(log, *file, "internal parser error: expeced ident but got none");
          return parsed;
        }
        SkipSpaces(file);
        EXPECT('(');
        SkipSpaces(file);
        EXPECT(')');
        SkipSpaces(file);
        EXPECT(';');
        SkipSpaces(file);
      }
      else
      {
        Add(log, *file, "unknown char found: " + CharToString(first));
        return parsed;
      }
    }

    return parsed;
#undef EXPECT
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

