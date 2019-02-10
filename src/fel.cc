#include "fel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

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
  }

  void Fel::LoadAndRunFile(const std::string& file, Log* log)
  {
    std::ifstream t(file.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    LoadAndRunString(str, file, log);
  }
}

