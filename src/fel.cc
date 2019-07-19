#include "fel.h"

#include <string>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

namespace fel
{
  struct Str
  {
    std::ostringstream ss;

    template<typename T>
    Str& operator<<(const T& t)
    {
      ss << t;
      return *this;
    }

    operator const std::string() const
    {
      return ss.str();
    }
  };

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

  void Add(Log* log, const File& file, const std::string& message)
  {
    Add(log, file.filename, file.location, message);
  }

  void Add(Log* log, const File& file, const Location& location, const std::string& message)
  {
    Add(log, file.filename, location, message);
  }

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

  std::shared_ptr<std::string> ParseString(File* file, char quote)
  {
    if(file->Peek() != quote)
    {
      return nullptr;
    }
    file->Read(); // quote
    std::ostringstream ss;
    while(file->HasMore())
    {
      char c = file->Read();
      if(c == quote)
      {
        return std::make_shared<std::string>(ss.str());
      }
      if(c != '\\')
      {
        switch(c)
        {
          case '\r': case '\n': case '\t':
            return nullptr;
          default:
            ss << c;
        }
      }
      else
      {
        if(!file->HasMore())
        {
          return nullptr;
        }
        const auto next = file->Read();
        switch(next)
        {
          case '\n': break;
          case '\'': case '\"': case '\\':
            ss << next;
            break;
          case 'n': ss << '\n'; break;
          case 'r': ss << '\r'; break;
          case 't': ss << '\t'; break;
          default:
            return nullptr;
        }
      }
    }
    return nullptr;
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

  bool SkipMultilineComment(File* file, Log* log)
  {
    const auto start_location = file->location;
    const auto first = file->Read();
    const auto second = file->Read();
    if(first != '/' || second != '*')
    {
      Add(log, *file, "Invalid start of multiline comment");
      return false;
    }
    while(file->HasMore())
    {
      const auto next = file->Peek();
      const auto after = file->Peek(2);
      if(next == '/' && after == '*')
      {
        if(false == SkipMultilineComment(file, log))
        {
          return false;
        }
      }
      else if(next == '*' && after == '/')
      {
        file->Read(); // skip
        file->Read(); // skip
        return true;
      }
      else
      {
        file->Read(); // skip
      }
    }
    
    Add(log, *file, "Multiline comment not ended before EOF");
    Add(log, *file, start_location, "Comment started here");
    return false;
  }

  bool SkipSinglelineComment(File* file, Log* log)
  {
    const auto first = file->Read();
    const auto second = file->Read();
    if(first != '/' || second != '/')
    {
      Add(log, *file, "Invalid start of one line comment");
      return false;
    }
    while(file->HasMore() && file->Peek() != '\n')
    {
      file->Read(); // skip
    }
    return true;
  }


  bool SkipSpaces(File* file, Log* log)
  {
    while(file->HasMore())
    {
      const auto c = file->Peek();
      if(IsSpace(c))
      {
        file->Read(); // skip
      }
      else if(c == '/')
      {
        const auto next = file->Peek(2);
        if(next == '*')
        {
          if(false == SkipMultilineComment(file, log))
          {
            return false;
          }
        }
        else if(next == '/')
        {
          if(false == SkipSinglelineComment(file, log))
          {
            return false;
          }
        }
        else
        {
          return true;
        }
      }
      else
      {
        return true;
      }
    }
    return true;
  }

  struct Rvalue
  {
    Location location;
    virtual ~Rvalue() {}
    virtual std::shared_ptr<Value> CalculateValue(const std::string& filename, Log* log) = 0;
  };

  struct AddRvalue : public Rvalue
  {
    Location location;
    std::shared_ptr<Rvalue> lhs;
    std::shared_ptr<Rvalue> rhs;

    AddRvalue(const Location& lo, std::shared_ptr<Rvalue> l, std::shared_ptr<Rvalue> r) : location(lo), lhs(l), rhs(r) {}
    virtual ~AddRvalue() {}

    std::shared_ptr<Value> CalculateValue(const std::string& filename, Log* log)
    {
      auto lhs_value = lhs->CalculateValue(filename, log);
      if(lhs_value == nullptr) { return nullptr; }
      auto rhs_value = rhs->CalculateValue(filename, log);
      if(rhs_value == nullptr) { return nullptr; }

      auto* lhs_value_int = lhs_value->AsInt();
      if(lhs_value_int != nullptr)
      {
        auto* rhs_value_int = rhs_value->AsInt();
        if(rhs_value_int == nullptr)
        {
          Add(log, filename, location, Str() << "invalid operation for +: lhs is int but rhs is " << rhs_value->TypeToString());
          return nullptr;
        }
        return std::make_shared<IntValue>(lhs_value_int->value + rhs_value_int->value);
      }

      auto* lhs_value_string = lhs_value->AsString();
      if(lhs_value_string != nullptr)
      {
        auto* rhs_value_string = rhs_value->AsString();
        if(rhs_value_string == nullptr)
        {
          Add(log, filename, location, Str() << "invalid operation for +: lhs is string but rhs is " << rhs_value->TypeToString());
          return nullptr;
        }
        return std::make_shared<StringValue>(lhs_value_string->value + rhs_value_string->value);
      }

      Add(log, filename, location, Str() << "unknown operation for +: lhs is " << lhs_value->TypeToString() << " but rhs is " << rhs_value->TypeToString());
      return nullptr;
    }
  };


  struct ConstantRvalue : public Rvalue
  {
    std::shared_ptr<Value> value;

    explicit ConstantRvalue(std::shared_ptr<Value> v) : value(v) {}
    ~ConstantRvalue() {}

    std::shared_ptr<Value> CalculateValue(const std::string& filename, Log* log) override
    {
      return value;
    }
  };

  struct FunctionCall
  {
    std::string function;
    Location location;
    std::vector<std::shared_ptr<Rvalue>> arguments;
  };

  struct Parsed
  {
    std::string filename;
    std::vector<FunctionCall> statements;
  };

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

  std::shared_ptr<Value> ParseIntConstant(File* file, Log* log)
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

  std::shared_ptr<Value> ParseConstant(File* file, Log* log)
  {
    const auto p = file->Peek();
    if(IsNum(p))
    {
      return ParseIntConstant(file, log);
    }
    if(p == '\'' || p=='\"')
    {
      auto s = ParseString(file, p);
      if(s == nullptr)
      {
        Add(log, *file, "Unable to parse string, found " + CharToString(file->Peek()));
        return nullptr;
      }
      return std::make_shared<StringValue>(*s);
    }

    Add(log, *file, "Unknown constant value, found " + CharToString(p));
    return nullptr;
  }

  std::shared_ptr<Rvalue> ParseRval(File* file, Log* log)
  {
    auto val = ParseConstant(file, log);
    if(val == nullptr)
    {
      return nullptr;
    }
    auto rv = std::make_shared<ConstantRvalue>(val);
    if(false == SkipSpaces(file, log))
    {
      return nullptr;
    }
    if(file->Peek() == '+')
    {
      const auto l = file->location;
      file->Read(); // skip
      if(false == SkipSpaces(file, log))
      {
        return nullptr;
      }
      auto right = ParseRval(file, log);
      if(right == nullptr)
      {
        return nullptr;
      }
      return std::make_shared<AddRvalue>(l, rv, right);
    }
    return rv;
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

    if(false == SkipSpaces(file, log))
    {
      return parsed;
    }
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
        if(false == SkipSpaces(file, log))
        {
          return parsed;
        }
        EXPECT('(');
        if(false == SkipSpaces(file, log))
        {
          return parsed;
        }
        auto fc = FunctionCall{ident, loc};
        bool first = true;
        while(file->HasMore() && file->Peek() != ')')
        {
          if(!first)
          {
            EXPECT(',');
            if(false == SkipSpaces(file, log))
            {
              return parsed;
            }
          }
          first = false;
          if(file->Peek() == ')')
          {
            Add(log, *file, "Found ) while looking for rvalue");
            return parsed;
          }
          auto rv = ParseRval(file, log);
          if(rv == nullptr)
          {
            return parsed;
          }
          fc.arguments.push_back(rv);
        }
        EXPECT(')');
        if(false == SkipSpaces(file, log))
        {
          return parsed;
        }
        EXPECT(';');
        if(false == SkipSpaces(file, log))
        {
          return parsed;
        }
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
        return;
      }
      else
      {
        State state;
        for(const auto& a: s.arguments)
        {
          auto arg = a->CalculateValue(parsed.filename, log);
          if(arg == nullptr)
          {
            return;
          }
          state.stack.push_back(arg);
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

