#include "parser.h"

#include "lexer.h"

namespace fel
{
    struct Parser
    {
        LexerReader* lexer;
        Log* log;
    };

    bool ParseProgram(Parser* parser)
    {
        if( parser->lexer->Peek().type == TokenType::EndOfStream )
        return true;
        else return false;
    }

    bool Parse(LexerReader* reader, Log* log)
    {
        auto parser = Parser{reader, log};
        return ParseProgram(&parser);
    }
}
