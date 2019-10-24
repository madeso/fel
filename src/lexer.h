#ifndef FEL_LEXER_H
#define FEL_LEXER_H

#include <string>
#include <vector>

#include "file.h"

namespace fel
{
    enum class TokenType
    {
        Unknown,
        BeginBrace, EndBrace,
        OpenParen, CloseParen,
        OpenBracket, CloseBracket,
        
        Plus, Minus, Mult, Div, Comma, Colon, Term,

        Dot, DotDot,
        Equal, Assign,
        Less, LessEqual,
        Greater, GreaterEqual,

        String,

        Identifier,

        KeywordIf, KeywordElse, KeywordFor, KeywordFunction, KeywordVar,
        KeywordTrue, KeywordFalse, KeywordNull,

        Int, Number,
        
        EndOfStream
    };

    std::string ToString(const TokenType tt);

    struct Token
    {
        TokenType type;
        std::string text;
    };

    template<typename  Stream>
    Stream& operator<<(Stream& stream, const Token& token)
    {
        stream << ToString(token.type) << ": " << token.text;
        return stream;
    }

    struct Lexer
    {
        FilePointer file;

        explicit Lexer(const File& a_file);

        Token
        GetNextToken();
    };

    std::vector<Token> GetAllTokensInFile(const File& file);

}

#endif  // FEL_LEXER_H
