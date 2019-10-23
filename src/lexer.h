#ifndef FEL_LEXER_H
#define FEL_LEXER_H

#include <string>

#include "file.h"

namespace fel
{
    enum class TokenType
    {
        Unknown,
        BeginBrace, EndBrace,
        OpenParen, CloseParen,
        OpenBracket, CloseBracket,
        
        Plus, Minus, Mult, Div, Comma, Dot, Colon, Term,

        Equal, Assign, Less, Greater, LessEqual, GreaterEqual,

        String,

        Identifier,

        KeywordIf, KeywordElse, KeywordFor, KeywordFunction, KeywordVar,
        KeywordTrue, KeywordFalse, KeywordNull,

        Int, Number,
        
        EndOfStream
    };

    struct Token
    {
        TokenType type;
        std::string text;
    };

    struct Lexer
    {
        FilePointer file;

        explicit Lexer(const File& a_file);

        Token
        GetNextToken();
    };

}

#endif  // FEL_LEXER_H
