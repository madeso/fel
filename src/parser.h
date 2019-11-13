#ifndef FEL_PARSER_H
#define FEL_PARSER_H

#include "ast.h"

namespace fel
{
    struct LexerReader;
    struct Log;

    StatementPtr Parse(LexerReader* reader, Log* log);
}

#endif  // FEL_PARSER_H
