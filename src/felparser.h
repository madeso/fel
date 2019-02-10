#ifndef FEL_PARSER_H
#define FEL_PARSER_H

#include <string>

#include "fel.h"

struct FelState
{
  int first_column = 0;
  int last_column = 0;
  std::string file;

  void AddLog(const std::string& file, int line, int col, const std::string& message);

  fel::Log* log;
};

#endif // FEL_PARSER_H

