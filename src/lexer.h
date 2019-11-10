#ifndef FEL_LEXER_H
#define FEL_LEXER_H

#include <string>
#include <vector>
#include <optional>

#include "file.h"

namespace fel
{
    struct Log;

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
        KeywordTrue, KeywordFalse, KeywordNull, KeywordReturn,

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
        Log* log;

        Lexer(const File& a_file, Log* a_log);

        Token
        GetNextToken();
    };

    struct LexerReader
    {
        Lexer lexer;
        std::optional<Token> token;

        LexerReader(const File& a_file, Log* a_log);

        Token Peek();
        Token Read();
    };

    std::vector<Token> GetAllTokensInFile(LexerReader* reader);
}

#endif  // FEL_LEXER_H
