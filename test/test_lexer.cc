#include "catch.hpp"

#include <sstream>
#include <iostream>
#include "lexer.h"
#include "file.h"
#include "log.h"

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
    Stream& operator<<(Stream& stream, const TestToken& tt)
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
            Tokenize(S("  // cats are cool"), &log),
            Equals(empty)
        );
    }

    SECTION("operators")
    {
        CHECK_THAT
        (
            Tokenize(S("(..)"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::OpenParen,
                    TokenType::DotDot,
                    TokenType::CloseParen
                }
            )
        );
        
        CHECK_THAT
        (
            Tokenize(S("[if]"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::OpenBracket,
                    TokenType::KeywordIf,
                    TokenType::CloseBracket
                }
            )
        );
        
        CHECK_THAT
        (
            Tokenize(S("{.}"), &log),
            Equals<TestToken>
            (
                {
                    TokenType::BeginBrace,
                    TokenType::Dot,
                    TokenType::EndBrace
                }
            )
        );
    }
}
