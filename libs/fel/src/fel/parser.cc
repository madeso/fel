#include "fel/parser.h"

#include "fel/lexer.h"
#include "fel/ast.h"
#include "fel/log.h"

namespace fel
{
    Parser::Parser(const File& file, Log* l)
        : reader(file, l)
        , log(l)
    {
    }


    Expr
    Parser::Parse()
    {
        try
        {
            return ParseExpression();
        }
        catch (const ParseError&)
        {
            return nullptr;
        }
    }


    Expr
    Parser::ParseExpression()
    {
        return ParseEquality();
    }


    Expr
    Parser::ParseEquality()
    {
        Expr expr = ParseComparison();

        while(ParseMatch({TokenType::NotEqual, TokenType::Equal}))
        {
            auto op = GetPreviousToken();
            auto right = ParseComparison();
            expr = std::make_shared<BinaryExpression>(expr, op, right);
        }

        return expr;
    }


    Expr
    Parser::ParseComparison()
    {
        Expr expr = ParseTerm();

        while(ParseMatch({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual}))
        {
            auto op = GetPreviousToken();
            auto right = ParseTerm();
            expr = std::make_shared<BinaryExpression>(expr, op, right);
        }

        return expr;
    }

    Expr
    Parser::ParseTerm()
    {
        Expr expr = ParseFactor();

        while(ParseMatch({TokenType::Minus, TokenType::Plus}))
        {
            auto op = GetPreviousToken();
            auto right = ParseFactor();
            expr = std::make_shared<BinaryExpression>(expr, op, right);
        }

        return expr;
    }


    Expr
    Parser::ParseFactor()
    {
        Expr expr = ParseUnary();

        while(ParseMatch({TokenType::Div, TokenType::Mult}))
        {
            auto op = GetPreviousToken();
            auto right = ParseUnary();
            expr = std::make_shared<BinaryExpression>(expr, op, right);
        }

        return expr;
    }


    Expr
    Parser::ParseUnary()
    {
        if (ParseMatch({TokenType::Not, TokenType::Minus}))
        {
            auto op = GetPreviousToken();
            auto right = ParseUnary();
            return std::make_shared<UnaryExpression>(op, right);
        }

        return ParsePrimary();
    }


    Expr
    Parser::ParsePrimary()
    {
        if(ParseMatch({TokenType::KeywordFalse})) return std::make_shared<LiteralExpression>(Object::FromBool(false), GetPreviousToken().where);
        if(ParseMatch({TokenType::KeywordTrue})) return std::make_shared<LiteralExpression>(Object::FromBool(true), GetPreviousToken().where);
        if(ParseMatch({TokenType::KeywordNull})) return std::make_shared<LiteralExpression>(nullptr, GetPreviousToken().where);

        if(ParseMatch({TokenType::Int, TokenType::Number, TokenType::String}))
        {
            return std::make_shared<LiteralExpression>(GetPreviousToken().literal, GetPreviousToken().where);
        }

        if(ParseMatch({TokenType::OpenParen}))
        {
            auto expr = ParseExpression();
            Consume(TokenType::CloseParen, log::Type::MissingCloseParen);
            return std::make_shared<GroupingExpression>(expr);
        }


        // todo(Gustav): how to handle?
        throw Error(Peek(), log::Type::ExpectedExpression);
    }


    Token
    Parser::Consume(TokenType token_type, const log::Type log_type, const std::vector<std::string>& args)
    {
        if(Check(token_type))
        {
            return Advance();
        }

        throw Error(Peek(), log_type, args);
        // return Token{TokenType::EndOfStream, "<invalid token>", nullptr, {}};
    }

    ParseError
    Parser::Error(Token token, const log::Type type, const std::vector<std::string>& args)
    {
        ReportError(token, type, args);
        return ParseError {};
    }


    void
    Parser::ReportError(Token token, const log::Type type, const std::vector<std::string>& args)
    {
        log->AddError(token.where, type, args);
    }


    bool
    Parser::ParseMatch(const std::vector<TokenType> types)
    {
        for (auto type : types)
        {
            if(Check(type))
            {
                Advance();
                return true;
            }
        }

        return false;
    }


    void
    Parser::Synchronize()
    {
        Advance();

        while (!IsAtEnd())
        {
            if (GetPreviousToken().type == TokenType::Term)
            {
                return;
            }

            switch (Peek().type)
            {
                case TokenType::KeywordFunction:
                case TokenType::KeywordVar:
                case TokenType::KeywordFor:
                case TokenType::KeywordIf:
                case TokenType::KeywordWhile:
                case TokenType::KeywordPrint:
                case TokenType::KeywordReturn:
                    return;

                default:
                    break;
            }

            Advance();
        }
    }


    // returns true if the current token is of the given type
    bool
    Parser::Check(TokenType type)
    {
        if (IsAtEnd()) return false;
        return Peek().type == type;
    }


    Token
    Parser::Peek()
    {
        return reader.Peek();
    }


    Token
    Parser::Advance()
    {
        return reader.Read();
    }

    Token
    Parser::GetPreviousToken()
    {
        return *reader.previous;
    }


    bool
    Parser::IsAtEnd()
    {
        return reader.Peek().type == TokenType::EndOfStream;
    }
}
