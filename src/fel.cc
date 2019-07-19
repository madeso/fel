#include "fel.h"

#include <string>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

namespace fel
{
  struct File
  {
    std::string filename;
    std::string data;
    int next_index = 0;
    Location location = Location{1,0};
    
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
          location.line += 1;
          location.column = 0;
        }
        else
        {
          location.column += 1;
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

  std::string ParseInt(File* file)
  {
    if(!IsNum(file->Peek()))
    {
      return "";
    }
    std::ostringstream ss;
    const auto first = file->Read();
    ss << first;
    if(first == '0')
    {
      // todo: support octal numbers
      return ss.str();
    }

    while(file->HasMore())
    {
      const auto p = file->Peek();
      if(IsNum(p))
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

  struct RValue
  {
    std::shared_ptr<Value> value;
  };


  struct FunctionCall
  {
    std::string function;
    Location location;
    std::vector<RValue> arguments;
  };

  struct Parsed
  {
    std::string filename;
    std::vector<FunctionCall> statements;
  };

  void Add(Log* log, const File& file, const std::string& message)
  {
    Add(log, file.filename, file.location, message);
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

  std::shared_ptr<Value> ParseConstant(File* file, Log* log)
  {
    auto int_string = ParseInt(file);
    if(int_string == "" )
    {
      Add(log, *file, "Failed to parse int, found " + CharToString(file->Peek()));
      return nullptr;
    }
    std::istringstream ss (int_string);
    int int_parsed;
    ss >> int_parsed;
    if(ss.fail())
    {
      Add(log, *file, "Internal error: Unable to get int from " + int_string);
      return nullptr;
    }
    return std::make_shared<IntValue>(int_parsed);
  }

  Parsed Parse(File* file, Log* log)
  {
    Parsed parsed;
    parsed.filename = file->filename;

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
        const auto loc = file->location;
        const auto ident = ParseIdent(file);
        if(ident == "")
        {
          Add(log, *file, "internal parser error: expeced ident but got none");
          return parsed;
        }
        SkipSpaces(file);
        EXPECT('(');
        SkipSpaces(file);
        auto fc = FunctionCall{ident, loc};
        bool first = true;
        while(file->HasMore() && file->Peek() != ')')
        {
          if(!first)
          {
            EXPECT(',');
            SkipSpaces(file);
          }
          first = false;
          if(file->Peek() == ')')
          {
            Add(log, *file, "Found ) while looking for rvalue");
            return parsed;
          }
          auto val = ParseConstant(file, log);
          if(val == nullptr)
          {
            return parsed;
          }
          fc.arguments.push_back(RValue{val});
          SkipSpaces(file);
        }
        EXPECT(')');
        SkipSpaces(file);
        EXPECT(';');
        SkipSpaces(file);
        parsed.statements.push_back(fc);
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

  void Run(const Fel& fel, const Parsed& parsed, Log* log)
  {
    for(auto& s: parsed.statements)
    {
      auto found = fel.functions.find(s.function);
      if(found == fel.functions.end())
      {
        Add(log, parsed.filename, s.location, s.function + " is not a function");
      }
      else
      {
        State state;
        for(const auto& a: s.arguments)
        {
          state.stack.push_back(a.value);
          state.arguments += 1;
        }
        found->second(&state);
      }
    }
  }

  std::shared_ptr<Value> State::GetStack(int index) const
  {
    assert(index < 0);
    return stack[stack.size()+index];
  }

  std::shared_ptr<Value> State::GetArg(int index) const
  {
    assert(index >= 0);
    assert(index < arguments);
    return GetStack(index - arguments);
  }

  void Fel::SetFunction(const std::string& name, FunctionCallback callback)
  {
    functions[name] = callback;
  }

  void Fel::LoadAndRunString(const std::string& str, const std::string& filename, Log* log)
  {
    auto file = File{filename, str};
    auto parsed = Parse(&file, log);
    if(!IsEmpty(*log)) { return; }

    Run(*this, parsed, log);
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
    std::ifstream t(file.c_str());
    if(!t.good())
    {
      Add(log, file, Location{-1,-1}, "Unable to open file!");
      return;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    LoadAndRunString(str, file, log);
  }
}

