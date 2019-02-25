#include "fel.h"
#include "bytecode.h"

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
      : static_cast<int>(stack.size())+index
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

  void State::Push(const std::string& str)
  {
    stack.push_back(Entry{str});
  }

  void State::Pop(int count)
  {
    for(int i=0;i<count; i+=1)
    {
      stack.pop_back();
    }
  }
  
  void Fel::SetFunction(const std::string& name, Fel::Callback callback)
  {
    functions[name] = callback;
  }

  struct StackPush : public ValueVisitor
  {
    StackPush(Compiler* s) : run_state(s) {}

    Compiler* run_state;
    void OnString(const StringValue& str) override
    {
      run_state->PushString(str.value);
    }
  }; 

  struct CompileStatements : public StatementVisitor
  {
    CompileStatements(fel::Fel* f, Compiler* r, fel::Log* l, const std::string& fn)
      : fel(f)
      , run_state(r)
      , log(l)
      , filename(fn)
    {}
    
    fel::Fel* fel;
    Compiler* run_state;
    fel::Log* log;
    std::string filename;

    void OnFunctionCall(const FunctionCall& fc) override
    {
      auto pusher = StackPush{run_state};
      fc.arguments->Visit(&pusher);
      run_state->CallFunction(fc.name, 1);
      run_state->Pop(1);
    }
  };

  void Run(const CompiledCode& code, Fel* fel)
  {
    fel::State run_state;

    for(const auto& c: code.codes)
    {
      switch(c.operation)
      {
        case Operation::PushConstantString:
          run_state.Push(code.strings[c.argument]);
          break;
        case Operation::Pop:
          run_state.Pop(c.argument);
          break;
        case Operation::CallFunction:
          {
            const auto function_name = run_state.as_string(-1);
            run_state.Pop(1);
            auto function = fel->functions.find(function_name);
            if(function == fel->functions.end())
            {
              return;
            }
            else
            {
              function->second(c.argument, &run_state);
            }
          }
          break;
      }
    }
  }

  void Fel::LoadAndRunString(const std::string& str, const std::string& filename, Log* log)
  {
    StatementList program;
    StringToAst(str, filename, log, &program);

    // run state
    // todo: generate bytecode and run that instead
    CompiledCode code;
    {
      auto compiler = Compiler(&code);
      auto runner = CompileStatements{this, &compiler, log, filename};
      program.VisitAll(&runner);
    }
    Run(code, this);
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
    std::ifstream t(file.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    LoadAndRunString(str, file, log);
  }
}

