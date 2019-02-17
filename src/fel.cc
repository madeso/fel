#include "fel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

#include "felparser.h"
#include "parser.hh"
#include "lexer.hh"

void FelState::AddLog(const std::string& file, int line, int col, const std::string& message)
{
  fel::LogEntry entry;
  entry.file = file;
  entry.message = message;
  entry.line = line;
  entry.column = col;
  log->entries.push_back(entry);
}
  
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

  void Fel::LoadAndRunString(const std::string& str, const std::string& filename, Log* log)
  {
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        /* could not initialize */
        return;
    }

    FelState fel;
    fel.file = filename;
    fel.log = log;
    yyset_extra(&fel, scanner);

    state = yy_scan_string(str.c_str(), scanner);
    yyset_lineno(1, scanner);

    if (yyparse(scanner, &fel)) {
        /* error parsing */
        return;
    }

    yy_delete_buffer(state, scanner);
    yylex_destroy(scanner);
    
    // run ast
    // this is some ugly shit, it's highly temporary but it works for now...
    fel::State run_state;
    const auto& program = fel.program;
    for(const auto& st : program.statements)
    {
      auto* fc = static_cast<FunctionCall*>(st.get());
      auto found = functions.find(fc->name);
      if(found == functions.end())
      {
        fel.AddLog(filename, -1, -1, "Unknown function");
      }
      else
      {
        run_state.stack.push_back(Entry{static_cast<StringValue*>(fc->arguments.get())->value});
        found->second(1, &run_state);
        run_state.stack.pop_back();
      }
    }
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
    std::ifstream t(file.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    LoadAndRunString(str, file, log);
  }
}

