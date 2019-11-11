#include "parser.h"

#include "lexer.h"
#include "log.h"

namespace
{
    using namespace fel;

    std::string ToString(Token token)
    {
        switch(token.type)
        {
        case TokenType::String:
            return "string";
        case TokenType::Identifier:
            return "ident: " + token.text;
        case TokenType::EndOfStream:
            return "<end of stream>";
        default:
            return token.text;
        }
    }

    struct Parser
    {
        LexerReader* lexer;
        Log* log;

        bool Error(log::Type error, const std::vector<std::string>& args)
        {
            log->AddError(lexer->lexer.file, error, args);
            return false;
        }

        Token Peek() const
        {
            return lexer->Peek();
        }

        bool Accept(TokenType token)
        {
            if(Peek().type == token)
            {
                lexer->Read();
                return true;
            }
            else
            {
                return false;
            }
        }

        bool Require(TokenType token)
        {
            if(Accept(token))
            {
                return true;
            }
            else
            {
                Error(log::Type::UnexpectedSymbol, { fel::ToString(token), ToString(lexer->Peek()) });
                return false;
            }
        }
    };

    bool ParseValue(Parser* parser);

    bool ParseValueArguments(Parser* parser)
    {
        if(!ParseValue(parser)) {return false;}
        while(parser->Accept(TokenType::Comma))
        {
            if(!ParseValue(parser)) {return false;}
        }

        return true;
    }

    bool ParseSimpleValue(Parser* parser)
    {
        if(parser->Accept(TokenType::KeywordTrue)) { return true; }
        if(parser->Accept(TokenType::KeywordFalse)) { return true; }
        if(parser->Accept(TokenType::KeywordNull)) { return true; }
        if(parser->Accept(TokenType::Int)) { return true; }
        if(parser->Accept(TokenType::Number)) { return true;}
        if(parser->Accept(TokenType::String)) { return true;}
        if(parser->Accept(TokenType::Identifier))
        {
            while(true)
            {
                if(parser->Accept(TokenType::OpenParen))
                {
                    // function call
                    if(parser->Peek().type != TokenType::CloseParen)
                    {
                        if(!ParseValueArguments(parser)) { return false;}
                    }
                    if(!parser->Require(TokenType::CloseParen)) {return false;}
                }
                else if (parser->Accept(TokenType::OpenBracket))
                {
                    // array index
                    if(!ParseValueArguments(parser)) { return false;}
                    if(!parser->Require(TokenType::CloseBracket)) {return false;}
                }
                else
                {
                    return true;
                }
            }
        }
        return parser->Error(log::Type::InvalidParserState, {"simple value", ToString(parser->Peek())});
    }

    bool ParseStatement(Parser* parser);

    bool ParseValue(Parser* parser)
    {
        if(parser->Accept(TokenType::OpenParen))
        {
            if(!ParseValue(parser)) {return false;}
            if(parser->Require(TokenType::CloseParen)) {return false;}
            return true;
        }
        if(parser->Accept(TokenType::KeywordFunction))
        {
            if(!parser->Require(TokenType::OpenParen)){ return false; }
            if(!parser->Require(TokenType::CloseParen)){ return false; }
            if(!ParseStatement(parser)) { return false;}
            return true;
        }

        if(!ParseSimpleValue(parser)) { return false; }

        while(parser->Accept(TokenType::Dot))
        {
            if(!ParseSimpleValue(parser)) { return false; }
        }

        return true;
    }

    bool ParseManyStatements(Parser* parser);

    bool ParseStatement(Parser* parser)
    {
        #define term() do { if(!parser->Require(TokenType::Term)) {return false;} } while(false)
        if(parser->Accept(TokenType::Term)) {return true;}
        if(parser->Accept(TokenType::BeginBrace))
        {
            if(!ParseManyStatements(parser)) { return false;}
            if(!parser->Require(TokenType::EndBrace)) {return false;}
            return true;
        }
        if( parser->Accept(TokenType::KeywordVar) )
        {
            if(!parser->Require(TokenType::Identifier)) {return false;}
            if(!parser->Require(TokenType::Assign)) {return false;}
            if(!ParseValue(parser)) {return false;}
            term();
            return true;
        }
        if(parser->Accept(TokenType::KeywordReturn))
        {
            if(parser->Accept(TokenType::Term))
            {
                // single return statement
                return true;
            }

            if(!ParseValue(parser)) {return false;}

            term();
            return true;
        }
        if( parser->Accept(TokenType::KeywordIf) )
        {
            if(!parser->Require(TokenType::OpenParen)) {return false;}
            if(!ParseValue(parser)) {return false;}
            if(!parser->Require(TokenType::CloseParen)) {return false;}
            if(!ParseStatement(parser)) {return false;}
            return true;
        }
        // else it's just a value like a function call, or a assign statement
        if(!ParseValue(parser)) return false;
        if(parser->Accept(TokenType::Assign))
        {
            if(!ParseValue(parser)) { return false; }
        }
        term();
        return true;
    }

    bool ParseManyStatements(Parser* parser)
    {
        while(parser->Peek().type != TokenType::EndOfStream && parser->Peek().type != TokenType::EndBrace)
        {
            if(ParseStatement(parser) == false) return false;
        }
        return true;
    }

    bool ParseProgram(Parser* parser)
    {
        const auto statements = ParseManyStatements(parser);
        if( parser->Peek().type == TokenType::EndOfStream )
        {
            return statements;
        }
        return parser->Error(log::Type::InvalidParserState, {"program", ToString(parser->Peek())});
    }
}

namespace fel
{
    bool Parse(LexerReader* reader, Log* log)
    {
        auto parser = Parser{reader, log};
        return ParseProgram(&parser);
    }
}
