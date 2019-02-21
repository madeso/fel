#include "fel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

#include "ast.h"

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


  const Entry& State::from_index(int index) const
  {
    assert(!stack.empty());
    assert(abs(index) < stack.size());
    const int i = (index >= 0)
      ? index
      : static_cast<int>(stack.size())-index
      ;
    assert(i >= 0 && i < stack.size());
    return stack[i];
  }

  bool State::is_string(int index) const
  {
    return true;
  }

  const std::string& State::as_string(int index) const
  {
    const auto& e = from_index(index);
    return e.str;
  }
  
  void Fel::SetFunction(const std::string& name, Fel::Callback callback)
  {
    functions[name] = callback;
  }

  struct StackPush : public ValueVisitor
  {
    StackPush(fel::State* s) : run_state(s) {}

    fel::State* run_state;
    void OnString(const StringValue& str) override
    {
      run_state->stack.push_back(Entry{str.value});
    }
  }; 

  struct RunStatements : public StatementVisitor
  {
    RunStatements(fel::Fel* f, fel::State* r, fel::Log* l, const std::string& fn)
      : fel(f)
      , run_state(r)
      , log(l)
      , filename(fn)
    {}
    
    fel::Fel* fel;
    fel::State* run_state;
    fel::Log* log;
    std::string filename;

    void OnFunctionCall(const FunctionCall& fc) override
    {
      auto found = fel->functions.find(fc.name);
      if(found == fel->functions.end())
      {
        log->AddLog(filename, -1, -1, "Unknown function");
      }
      else
      {
        auto pusher = StackPush{run_state};
        fc.arguments->Visit(&pusher);
        found->second(1, run_state);
        run_state->stack.pop_back();
      }

    }
  };

  void Fel::LoadAndRunString(const std::string& str, const std::string& filename, Log* log)
  {
    StatementList program;
    StringToAst(str, filename, log, &program);

    // run state
    // todo: generate bytecode and run that instead
    fel::State run_state;
    auto runner = RunStatements{this, &run_state, log, filename};
    program.VisitAll(&runner);
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
    std::ifstream t(file.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    LoadAndRunString(str, file, log);
  }
}

