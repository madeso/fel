#ifndef FEL_PARSER_H
#define FEL_PARSER_H

#include <string>

#include "fel.h"
#include "ast.h"

struct FelState
{
  int first_column = 0;
  int last_column = 0;
  std::string file;
  std::vector<char> str;

  StatementList program;

  void AddLog(const std::string& file, int line, int col, const std::string& message);

  fel::Log* log;
};

#endif // FEL_PARSER_H

