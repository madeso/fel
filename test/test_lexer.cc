#include "catch.hpp"

#include <sstream>
#include <iostream>
#include "fel/lexer.h"
#include "fel/file.h"
#include "fel/log.h"

using namespace fel;
using Catch::Matchers::Equals;

namespace
{
    File
    S(const std::string& source)
    {
        return {"source", source};
    }


    struct TestToken
    {
        TestToken(const Token& token)
            : type(token.type)
            , text(token.text)
        {
        }

        TestToken(TokenType tt)
            : type(tt)
        {
        }

        std::optional<TokenType> type;
        std::optional<std::string> text;
    };


    template<typename Stream>
    Stream&
    operator<<(Stream& stream, const TestToken& tt)
    {
        if(tt.type) { stream << ToString(*tt.type); }
        else { stream << "<no type>"; }
        
        stream << " " << tt.text.value_or("<no text>");
        
        return stream;
    }


    bool
    operator==(const TestToken& lhs, const TestToken& rhs)
    {
        if(lhs.type && lhs.type != rhs.type) { return false; }
        if(lhs.text && lhs.text != rhs.text) { return false; }
        return true;
    }


    bool
    operator!=(const TestToken& lhs, const TestToken& rhs)
    {
        return !(lhs == rhs);
    }


    std::vector<TestToken>
    C(const std::vector<Token>& tokens)
    {
        std::vector<TestToken> r;
        
        for(auto t: tokens) { r.emplace_back(t); }
        
        return r;
    }


    std::vector<TestToken>
    Tokenize(const File& a_file, Log* a_log)
    {
        auto lexer = LexerReader{a_file, a_log};
        return C(GetAllTokensInFile(&lexer));
    }
}


TEST_CASE("lexer", "[lexer]")
{
    Log log;
    SECTION("empty")
    {
        const auto empty = std::vector<TestToken>{};

        CHECK_THAT
        (
            Tokenize(S(""), &log),
            Equals(empty)
        );
        
        CHECK_THAT
        (
            Tokenize(S(" "), &log),
            Equals(empty)
        );
        
        CHECK_THAT
        (
            Tokenize(S("\n\t\n"), &log),
            Equals(empty)
        );

        CHECK_THAT
        (
            Tokenize(S("  /* all dogs are nice */  "), &log),
            Equals(empty)
        );
        
        CHECK_THAT
        (
            Tokenize(S("  /* all /* dogs */ are nice */  "), &log),
            Equals(empty)
        );
        
        CHECK_THAT
        (
            Tokenize(S("  // cats are cool"), &log),
            Equals(empty)
        );
    }

    SECTION("error")
    {
        CHECK_THAT
        (
            Tokenize(S("^"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::Unknown
                }
            )
        );

        // todo(Gustav): check errors
        CHECK_FALSE(log.IsEmpty());
    }

    // todo(Gustav): add utf8 tests, valid in stings, invalid outside

    SECTION("operators")
    {
        CHECK_THAT
        (
            Tokenize(S("{}()[]"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::BeginBrace,
                    TokenType::EndBrace,
                    TokenType::OpenParen,
                    TokenType::CloseParen,
                    TokenType::OpenBracket,
                    TokenType::CloseBracket
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S("+ - * / , : ;"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::Plus,
                    TokenType::Minus,
                    TokenType::Mult,
                    TokenType::Div,
                    TokenType::Comma,
                    TokenType::Colon,
                    TokenType::Term
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S(". .. == = < <= > >="), &log),
            Equals<TestToken>
            (
                {
                    TokenType::Dot,
                    TokenType::DotDot,
                    TokenType::Equal,
                    TokenType::Assign,
                    TokenType::Less,
                    TokenType::LessEqual,
                    TokenType::Greater,
                    TokenType::GreaterEqual
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S("~ ! & && | ||"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::BitNot,
                    TokenType::Not,
                    TokenType::BitAnd,
                    TokenType::And,
                    TokenType::BitOr,
                    TokenType::Or
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S("\"dog\" 'cat'"), &log),
            Equals<TestToken>
            (
                {
                    Token{TokenType::String, "dog"},
                    Token{TokenType::String, "cat"}
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S("dog cat"), &log),
            Equals<TestToken>
            (
                {
                    Token{TokenType::Identifier, "dog"},
                    Token{TokenType::Identifier, "cat"}
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S("if else for fun var true false null return"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::KeywordIf,
                    TokenType::KeywordElse,
                    TokenType::KeywordFor,
                    TokenType::KeywordFunction,
                    TokenType::KeywordVar,
                    TokenType::KeywordTrue,
                    TokenType::KeywordFalse,
                    TokenType::KeywordNull,
                    TokenType::KeywordReturn
                }
            )
        );
        CHECK_THAT
        (
            Tokenize(S("42 2.4"), &log),
            Equals<TestToken>
            (
                {
                    Token{TokenType::Int, "42"},
                    Token{TokenType::Number, "2.4"}
                }
            )
        );
    }
}
