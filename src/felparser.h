#ifndef FEL_PARSER_H
#define FEL_PARSER_H

#include <string>

struct FelState
{
  int first_column = 0;
  int last_column = 0;
  std::string file;
};

#endif // FEL_PARSER_H

