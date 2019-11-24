#include "parser.h"

#include <optional>
#include <sstream>
#include <iostream>

#include "lexer.h"
#include "log.h"
#include "ast.h"
#include "context.h"

#include "preproc.h"

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
        bool debug;
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

    #define FEL_ADD_CONTEXT(parser, message) auto FEL_CONCAT(scope_, __LINE__) = Scope{&parser->context, message}

    ValuePtr ParseValue(Parser* parser);

    #define DEBUG(parser, message) do { if (parser->debug) { std::cout << message << "\n"; } } while(false)

    std::optional<ValueList> ParseValueArguments(Parser* parser)
    {
        DEBUG(parser, "parse value arguments");
        FEL_ADD_CONTEXT(parser, "value arguments");
        ValueList values;

        do
        {
            if(auto value = ParseValue(parser); value)
            {
                DEBUG(parser, "got value");
                values.push_back(value);
            }
            else
            {
                DEBUG(parser, "failed to parse arguments");
                return std::nullopt;
            }
        } while(parser->Accept(TokenType::Comma));

        DEBUG(parser, "got value arguments");
        return values;
    }

    StatementPtr ParseStatement(Parser* parser);

    // bool=true if callable was accepted, false if not
    // Callable might be null if it was accepted but there was a syntax error
    std::pair<bool, ValuePtr> AcceptCallable(Parser* parser)
    {
        DEBUG(parser, "accept callable");
        FEL_ADD_CONTEXT(parser, "accept callable");
        if(auto ident = parser->Accept(TokenType::Identifier); ident)
        {
            DEBUG(parser, "callable ident");
            return {true, std::make_shared<ValueIdent>(ident->text)};
        }

        if(parser->Accept(TokenType::KeywordFunction))
        {
            DEBUG(parser, "got function keyword");
            if(!parser->Require(TokenType::OpenParen)){ DEBUG(parser, "syntax error, missing open paren after fun"); return {true, nullptr}; }
            // todo(Gustav): parse argumentes!
            if(!parser->Require(TokenType::CloseParen)){ DEBUG(parser, "syntax error, missing close paren after fun"); return {true, nullptr}; }
            if(auto statement = ParseStatement(parser); statement)
            {
                DEBUG(parser, "parsed callable");
                return {true, std::make_shared<ValueFunctionDefinition>(statement)};
            }
            else
            {
                DEBUG(parser, "syntax error, missing statement after fun");
                return {true, nullptr};
            }
        }

        DEBUG(parser, "no callable");
        return {false, nullptr};
    }

    template<typename V>
    ValuePtr ParseNumber(Parser* parser, const Token& token)
    {
        DEBUG(parser, "parsing number");
        FEL_ADD_CONTEXT(parser, "number");
        decltype(V::value) value;
        std::istringstream iss(token.text.c_str());
        iss >> value;
        if(!iss.fail() && iss.eof()) { DEBUG(parser, "got number"); return std::make_shared<V>(value);}
        else
        {
            DEBUG(parser, "unable to parse number");
            parser->Error(log::Type::UnableToParseNumber, {token.text});
            return nullptr;
        }
    }

    ValuePtr ParseSimpleValue(Parser* parser)
    {
        DEBUG(parser, "parsing simple value");
        FEL_ADD_CONTEXT(parser, "simple value");
        if(parser->Accept(TokenType::KeywordTrue)) { DEBUG(parser, "got true"); return std::make_shared<ValueBool>(true); }
        if(parser->Accept(TokenType::KeywordFalse)) { DEBUG(parser, "got false"); return std::make_shared<ValueBool>(false); }
        if(parser->Accept(TokenType::KeywordNull)) { DEBUG(parser, "got null"); return std::make_shared<ValueNull>(); }
        if(auto value = parser->Accept(TokenType::Int); value) { DEBUG(parser, "got int"); return ParseNumber<ValueInt>(parser, *value); }
        if(auto value = parser->Accept(TokenType::Number); value) { DEBUG(parser, "got number"); return ParseNumber<ValueNumber>(parser, *value);}
        if(auto value = parser->Accept(TokenType::String); value) { DEBUG(parser, "got string"); return std::make_shared<ValueString>(value->text);}
        if(auto [callable_accepted, callable] = AcceptCallable(parser); callable_accepted)
        {
            DEBUG(parser, "got callable");
            FEL_ADD_CONTEXT(parser, "callable");
            if(!callable) { return nullptr; }

            auto ret = callable;
            while(true)
            {
                if(parser->Accept(TokenType::OpenParen))
                {
                    DEBUG(parser, "got open paren");
                    FEL_ADD_CONTEXT(parser, "function call");
                    // function call
                    ValueList arguments;
                    if(parser->Peek().type != TokenType::CloseParen)
                    {
                        if(auto optional_arguments = ParseValueArguments(parser); optional_arguments)
                        {
                            DEBUG(parser, "got function argument");
                            arguments = *optional_arguments;
                        }
                        else
                        {
                            DEBUG(parser, "failed to parse function argument");
                            return nullptr;
                        }
                    }
                    ret = std::make_shared<ValueCallFunction>(ret, arguments);
                    if(!parser->Require(TokenType::CloseParen)) {DEBUG(parser, "failed to get close parent"); return nullptr;}
                    DEBUG(parser, "function call done");
                }
                else if (parser->Accept(TokenType::OpenBracket))
                {
                    DEBUG(parser, "got open bracket");
                    FEL_ADD_CONTEXT(parser, "array index");
                    // array index
                    ValueList arguments;
                    FEL_ADD_CONTEXT(parser, "array index");
                    if(auto optional_arguments = ParseValueArguments(parser); optional_arguments)
                    {
                        arguments = *optional_arguments;
                    }
                    else
                    {
                        return nullptr;
                    }
                    ret = std::make_shared<ValueCallArray>(ret, arguments);
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
        FEL_ADD_CONTEXT(parser, "value");
        if(parser->Accept(TokenType::OpenParen))
        {
            FEL_ADD_CONTEXT(parser, "inside paren");
            auto value = ParseValue(parser);
            if(!value) {return nullptr;}
            if(parser->Require(TokenType::CloseParen)) {return nullptr;}
            return value;
        }
        
        auto value = ParseSimpleValue(parser);
        if(!value) { return nullptr; }

        while(parser->Accept(TokenType::Dot))
        {
            FEL_ADD_CONTEXT(parser, "dot");
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
        FEL_ADD_CONTEXT(parser, "statement");

        #define term() do { if(!parser->Require(TokenType::Term)) {return nullptr;} } while(false)
        if(parser->Accept(TokenType::Term)) {return std::make_shared<StatementNull>();}
        if(parser->Accept(TokenType::BeginBrace))
        {
            FEL_ADD_CONTEXT(parser, "inside braces");
            auto statements = ParseManyStatements(parser);
            if(!statements) { return nullptr;}
            if(!parser->Require(TokenType::EndBrace)) {return nullptr;}
            return statements;
        }
        if( parser->Accept(TokenType::KeywordVar) )
        {
            FEL_ADD_CONTEXT(parser, "var statement");
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
            FEL_ADD_CONTEXT(parser, "return statement");
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
            FEL_ADD_CONTEXT(parser, "if statement");
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
            FEL_ADD_CONTEXT(parser, "assign var");
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
        FEL_ADD_CONTEXT(parser, "many statements");

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
        FEL_ADD_CONTEXT(parser, "program");

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
    StatementPtr Parse(LexerReader* reader, Log* log, bool debug)
    {
        auto parser = Parser{reader, log, debug};
        return ParseProgram(&parser);
    }
}
