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

  void CompileFunctionCall(const FunctionCall& fc, Compiler* run_state, bool return_value);

  struct StackPush : public ValueVisitor
  {
    StackPush(Compiler* s) : run_state(s) {}

    Compiler* run_state;

    void OnFunctionCall(const FunctionCall& fc) override
    {
      CompileFunctionCall(fc, run_state, true);
    }

    void OnString(const StringValue& str) override
    {
      run_state->PushString(str.value);
    }
  }; 

  void CompileFunctionCall(const FunctionCall& fc, Compiler* run_state, bool return_value)
  {
    auto pusher = StackPush{run_state};
    for(auto a : fc.arguments->values)
    {
      a->Visit(&pusher);
    }
    const auto arguments = fc.arguments->values.size();
    if(return_value)
    {
      run_state->CallFunctionWithReturn(fc.name, arguments);
    }
    else
    {
      run_state->CallFunctionDiscardReturn(fc.name, arguments);
    }
    run_state->Pop(1);
  }

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
      CompileFunctionCall(fc, run_state, false);
    }
  };

  void Run(const CompiledCode& code, Fel* fel, Log* log)
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
        case Operation::CallFunctionDiscardReturn:
        case Operation::CallFunctionWithReturn:
          {
            const auto return_value = c.operation == Operation::CallFunctionWithReturn;
            const auto function_name = run_state.as_string(-1);
            run_state.Pop(1);
            auto function = fel->functions.find(function_name);
            if(function == fel->functions.end())
            {
              log->AddLog("file", -1, -1, "Unable to find function " + function_name);
              return;
            }
            else
            {
              const auto number_of_returns = function->second(c.argument, &run_state);
              if(return_value)
              {
                if(number_of_returns == 0)
                {
                  // function called and expected to return value, but no value was returned
                  // report error, null or something else?
                  run_state.Push("");
                }
                else
                {
                  assert(number_of_returns == 1);
                }
              }
              else
              {
                if(number_of_returns > 0)
                {
                  run_state.Pop(number_of_returns);
                }
              }
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
    Run(code, this, log);
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
    std::ifstream t(file.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    LoadAndRunString(str, file, log);
  }
}

