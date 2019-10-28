#ifndef FEL_PARSER_H
#define FEL_PARSER_H

namespace fel
{
    struct LexerReader;
    struct Log;

    bool Parse(LexerReader* reader, Log* log);
}

#endif  // FEL_PARSER_H
