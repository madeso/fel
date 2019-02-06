#include "fel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include "parser.hh"
#include "lexer.hh"

namespace fel
{
  void Fel::LoadAndRunString(const std::string& str, Log* log)
  {
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        /* could not initialize */
        return;
    }

    state = yy_scan_string(str.c_str(), scanner);

    if (yyparse(scanner)) {
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
    LoadAndRunString(str, log);
  }
}

