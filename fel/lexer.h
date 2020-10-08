#ifndef FEL_LEXER_H
#define FEL_LEXER_H

#include <string>
#include <vector>
#include <optional>

#include "fel/file.h"
#include "fel/object.h"


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

        Not, BitNot,
        And, Or, BitAnd, BitOr,

        String,

        Identifier,

        KeywordIf, KeywordElse, KeywordFor, KeywordFunction, KeywordVar,
        KeywordTrue, KeywordFalse, KeywordNull, KeywordReturn,

        Int, Number,
        
        EndOfStream
    };


    std::string
    ToString(const TokenType tt);


    struct Token
    {
        Token(TokenType t, const std::string& lex, std::shared_ptr<Object> lit, const Location& w);

        TokenType type;
        std::string lexeme;
        std::shared_ptr<Object> literal;
        Location where;
    };


    template<typename Stream>
    Stream& operator<<(Stream& stream, const Token& token)
    {
        stream << ToString(token.type) << ": " << token.lexeme;
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

        Token
        Peek();

        Token
        Read();
    };


    std::vector<Token>
    GetAllTokensInFile(LexerReader* reader);
}

#endif  // FEL_LEXER_H

