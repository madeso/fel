#include "parser.h"

#include "lexer.h"
#include "log.h"

namespace fel
{
    namespace
    {
        struct Parser
        {
            LexerReader* lexer;
            Log* log;
        };

        std::string ToString(Token token)
        {
            switch(token.type)
            {
            case TokenType::String:
                return "string";
            case TokenType::Identifier:
                return "ident: " + token.text;
            default:
                return token.text;
            }
        }

        void Error(Parser* parser, log::Type error, std::vector<std::string> args)
        {
            parser->log->AddError(parser->lexer->lexer.file, error, args);
        }

        bool Accept(Parser* parser, TokenType token)
        {
            if(parser->lexer->Peek().type == token)
            {
                parser->lexer->Read();
                return true;
            }
            else
            {
                return false;
            }
        }

        bool Expect(Parser* parser, TokenType token)
        {
            if(Accept(parser, token))
            {
                return true;
            }
            else
            {
                Error(parser, log::Type::UnexpectedSymbol, { fel::ToString(token), ToString(parser->lexer->Peek()) });
                return false;
            }
        }

        bool ParseStatement(Parser* parser)
        {
            // lvalue
            if( false == Expect(parser, TokenType::Identifier) ) { return false; }

            // function call
            if(Accept(parser, TokenType::OpenParen))
            {
                if(false == Expect(parser, TokenType::CloseParen)) {return false;}
                if(false == Expect(parser, TokenType::Term)) {return false;}
            }
            // assignment
            else if(Accept(parser, TokenType::Assign))
            {
                // rvalue
                if(false == Expect(parser, TokenType::Identifier)) {return false;}
                if(false == Expect(parser, TokenType::Term)) {return false;}
            }
            else
            {
                Error(parser, log::Type::InvalidSymbol, { ToString(parser->lexer->Peek()) });
                return false;
            }

            return true;
        }

        bool ParseProgram(Parser* parser)
        {
            if( parser->lexer->Peek().type != TokenType::EndOfStream )
            {
                if( ParseStatement(parser) == false)
                {
                    return false;
                }
            }
            return true;
        }
    }

    bool Parse(LexerReader* reader, Log* log)
    {
        auto parser = Parser{reader, log};
        return ParseProgram(&parser);
    }
}
