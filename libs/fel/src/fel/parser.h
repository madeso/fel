#ifndef FEL_PARSER_H
#define FEL_PARSER_H

#include <memory>
#include <vector>
#include <string>

#include "fel/lexer.h"
#include "fel/log.h"


namespace fel
{
    struct Expression;
    struct File;


    struct ParseError
    {
    };


    using Expr = std::shared_ptr<Expression>;


    struct Parser
    {
        LexerReader reader;
        Log* log;

        Parser(const File& file, Log* l);

        Expr
        Parse();

        Expr
        ParseExpression();

        Expr
        ParseEquality();

        Expr
        ParseComparison();

        Expr
        ParseTerm();

        Expr
        ParseFactor();

        Expr
        ParseUnary();

        Expr
        ParsePrimary();

        Token
        Consume(TokenType token_type, const log::Type log_type, const std::vector<std::string>& args = {});

        ParseError
        Error(Token token, const log::Type type, const std::vector<std::string>& args = {});

        void
        ReportError(Token token, const log::Type type, const std::vector<std::string>& args = {});


        bool
        ParseMatch(const std::vector<TokenType> types);

        void
        Synchronize();

        // returns true if the current token is of the given type
        bool
        Check(TokenType type);

        Token
        Peek();

        Token
        Advance();

        Token
        GetPreviousToken();

        bool
        IsAtEnd();
    };
}

#endif  // FEL_PARSER_H
