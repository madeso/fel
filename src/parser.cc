#include "parser.h"

#include <optional>
#include <sstream>

#include "lexer.h"
#include "log.h"
#include "ast.h"
#include "context.h"

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
        Context context = {};

        std::nullopt_t Error(log::Type error, const std::vector<std::string>& args)
        {
            log->AddError(lexer->lexer.file, error, args, context);
            return std::nullopt;
        }

        Token Peek() const
        {
            return lexer->Peek();
        }

        std::optional<Token> Accept(TokenType token)
        {
            if(Peek().type == token)
            {
                return lexer->Read();
            }
            else
            {
                return std::nullopt;
            }
        }

        std::optional<Token> Require(TokenType token)
        {
            if(auto r = Accept(token))
            {
                return r;
            }
            else
            {
                Error(log::Type::UnexpectedSymbol, { fel::ToString(token), ToString(lexer->Peek()) });
                return std::nullopt;
            }
        }
    };

    ValuePtr ParseValue(Parser* parser);

    std::optional<ValueList> ParseValueArguments(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "value arguments");
        ValueList values;

        do
        {
            if(auto value = ParseValue(parser); value)
            {
                values.push_back(value);
            }
            else
            {
                return std::nullopt;
            }
        } while(parser->Accept(TokenType::Comma));

        return values;
    }

    StatementPtr ParseStatement(Parser* parser);

    // bool=true if callable was accepted, false if not
    // Callable might be null if it was accepted but there was a syntax error
    std::pair<bool, ValuePtr> AcceptCallable(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "accept callable");
        if(auto ident = parser->Accept(TokenType::Identifier); ident)
        {
            return {true, std::make_shared<ValueIdent>(ident->text)};
        }

        if(parser->Accept(TokenType::KeywordFunction))
        {
            if(!parser->Require(TokenType::OpenParen)){ return {true, nullptr}; }
            // todo(Gustav): parse argumentes!
            if(!parser->Require(TokenType::CloseParen)){ return {true, nullptr}; }
            if(auto statement = ParseStatement(parser); statement)
            {
                return {true, std::make_shared<ValueFunctionDefinition>(statement)};
            }
            else
            {
                return {true, nullptr};
            }
        }

        return {false, nullptr};
    }

    template<typename V>
    ValuePtr ParseNumber(Parser* parser, const Token& token)
    {
        FEL_SCOPE(&parser->context, "number");
        decltype(V::value) value;
        std::istringstream iss(token.text.c_str());
        iss >> value;
        if(iss.good()) { return std::make_shared<V>(value);}
        else
        {
            parser->Error(log::Type::InvalidParserState, {"number", ToString(parser->Peek())});
            return nullptr;
        }
    }

    ValuePtr ParseSimpleValue(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "simple value");
        if(parser->Accept(TokenType::KeywordTrue)) { return std::make_shared<ValueBool>(true); }
        if(parser->Accept(TokenType::KeywordFalse)) { return std::make_shared<ValueBool>(false); }
        if(parser->Accept(TokenType::KeywordNull)) { return std::make_shared<ValueNull>(); }
        if(auto value = parser->Accept(TokenType::Int); value) { return ParseNumber<ValueInt>(parser, *value); }
        if(auto value = parser->Accept(TokenType::Number); value) { return ParseNumber<ValueNumber>(parser, *value);}
        if(auto value = parser->Accept(TokenType::String); value) { return std::make_shared<ValueString>(value->text);}
        if(auto [callable_accepted, callable] = AcceptCallable(parser); callable_accepted)
        {
            FEL_SCOPE(&parser->context, "callable");
            if(!callable) { return nullptr; }

            auto ret = callable;
            while(true)
            {
                if(parser->Accept(TokenType::OpenParen))
                {
                    FEL_SCOPE(&parser->context, "function call");
                    // function call
                    if(parser->Peek().type != TokenType::CloseParen)
                    {
                        if(auto arguments = ParseValueArguments(parser); arguments)
                        {
                            ret = std::make_shared<ValueCallFunction>(ret, *arguments);
                        }
                        else
                        {
                            return nullptr;
                        }
                    }
                    if(!parser->Require(TokenType::CloseParen)) {return nullptr;}
                }
                else if (parser->Accept(TokenType::OpenBracket))
                {
                    // array index
                    FEL_SCOPE(&parser->context, "array index");
                    if(auto arguments = ParseValueArguments(parser); arguments)
                    {
                        ret = std::make_shared<ValueCallArray>(ret, *arguments);
                    }
                    else
                    {
                        return nullptr;
                    }
                    if(!parser->Require(TokenType::CloseBracket)) {return nullptr;}
                }
                else
                {
                    return ret;
                }
            }
        }
        parser->Error(log::Type::InvalidParserState, {"simple value", ToString(parser->Peek())});
        return nullptr;
    }

    ValuePtr ParseValue(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "value");
        if(parser->Accept(TokenType::OpenParen))
        {
            FEL_SCOPE(&parser->context, "inside paren");
            auto value = ParseValue(parser);
            if(!value) {return nullptr;}
            if(parser->Require(TokenType::CloseParen)) {return nullptr;}
            return value;
        }
        
        auto value = ParseSimpleValue(parser);
        if(!value) { return nullptr; }

        while(parser->Accept(TokenType::Dot))
        {
            FEL_SCOPE(&parser->context, "dot");
            if(auto sub = ParseSimpleValue(parser); sub)
            {
                value = std::make_shared<ValueDotAccess>(value, sub);
            }
            else{ return nullptr; }
        }

        return value;
    }

    StatementPtr ParseManyStatements(Parser* parser);

    StatementPtr ParseStatement(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "statement");

        #define term() do { if(!parser->Require(TokenType::Term)) {return nullptr;} } while(false)
        if(parser->Accept(TokenType::Term)) {return std::make_shared<StatementNull>();}
        if(parser->Accept(TokenType::BeginBrace))
        {
            FEL_SCOPE(&parser->context, "inside braces");
            auto statements = ParseManyStatements(parser);
            if(!statements) { return nullptr;}
            if(!parser->Require(TokenType::EndBrace)) {return nullptr;}
            return statements;
        }
        if( parser->Accept(TokenType::KeywordVar) )
        {
            FEL_SCOPE(&parser->context, "var statement");
            auto name = parser->Require(TokenType::Identifier);
            if(!name) {return nullptr;}
            if(!parser->Require(TokenType::Assign)) {return nullptr;}
            auto value = ParseValue(parser);
            if(!value) {return nullptr;}
            term();
            return std::make_shared<StatementDeclaration>(name->text, value);
        }
        if(parser->Accept(TokenType::KeywordReturn))
        {
            FEL_SCOPE(&parser->context, "return statement");
            if(parser->Accept(TokenType::Term))
            {
                // single return statement
                return std::make_shared<StatementReturn>();
            }

            auto value = ParseValue(parser);
            if(!value) {return nullptr;}

            term();
            return std::make_shared<StatementReturnValue>(value);
        }
        if( parser->Accept(TokenType::KeywordIf) )
        {
            FEL_SCOPE(&parser->context, "if statement");
            if(!parser->Require(TokenType::OpenParen)) {return nullptr;}
            auto value = ParseValue(parser);
            if(!value) {return nullptr;}
            if(!parser->Require(TokenType::CloseParen)) {return nullptr;}
            auto statement = ParseStatement(parser);
            if(!statement) {return nullptr;}
            return std::make_shared<StatementConditionIf>(value, statement);
        }
        // else it's just a value like a function call, or a assign statement
        auto value = ParseValue(parser);
        if(!value) return nullptr;
        if(parser->Accept(TokenType::Assign))
        {
            FEL_SCOPE(&parser->context, "assign var");
            auto rhs = ParseValue(parser);
            if(!rhs) { return nullptr; }
            term();
            return std::make_shared<StatementAssign>(value, rhs);
        }
        else
        {
            term();
            return std::make_shared<StatementValue>(value);
        }
    }

    StatementPtr ParseManyStatements(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "many statements");

        auto list = std::make_shared<StatementList>();
        while(parser->Peek().type != TokenType::EndOfStream && parser->Peek().type != TokenType::EndBrace)
        {
            if(auto statement = ParseStatement(parser); statement)
            {
                list->statements.emplace_back(statement);
            }
            else
            {
                return nullptr;
            }
        }
        return list;
    }

    StatementPtr ParseProgram(Parser* parser)
    {
        FEL_SCOPE(&parser->context, "program");

        const auto statements = ParseManyStatements(parser);
        if( parser->Peek().type == TokenType::EndOfStream )
        {
            return statements;
        }
        parser->Error(log::Type::InvalidParserState, {"program", ToString(parser->Peek())});
        return nullptr;
    }
}

namespace fel
{
    StatementPtr Parse(LexerReader* reader, Log* log)
    {
        auto parser = Parser{reader, log};
        return ParseProgram(&parser);
    }
}
