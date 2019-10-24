#include "catch.hpp"

#include <sstream>
#include <iostream>
#include "lexer.h"
#include "file.h"

using namespace fel;
using Catch::Matchers::Equals;

namespace
{
    File S(const std::string& source)
    {
        return {"source", source};
    }

    struct TestToken
    {
        TestToken(const Token& token) : type(token.type), text(token.text)
        { }
        TestToken(TokenType tt) : type(tt)
        { }
        std::optional<TokenType> type;
        std::optional<std::string> text;
    };

    template<typename Stream>
    Stream& operator<<(Stream& stream, const TestToken& tt)
    {
        if(tt.type)
        {
            stream << ToString(*tt.type);
        }
        else
        {
            stream << "<no type>";
        }
        stream << " ";
        if(tt.text)
        {
            stream << *tt.text;
        }
        else
        {
            stream << "<no text>";
        }
        return stream;
    }

    bool operator==(const TestToken& lhs, const TestToken& rhs)
    {
        // std::cout << "LHS: " << lhs << "\n";
        // std::cout << "RHS: " << rhs << "\n";

        // lhs is matcher data

        if(lhs.type && lhs.type != rhs.type)
        {
            return false;
        }

        if(lhs.text && lhs.text != rhs.text)
        {
            return false;
        }
        
        return true;
    }
    bool operator!=(const TestToken& lhs, const TestToken& rhs)
    {
        return !(lhs == rhs);
    }

    std::vector<TestToken> C(const std::vector<Token> tokens)
    {
        std::vector<TestToken> r;
        for(auto t: tokens)
        {
            r.emplace_back(t);
        }
        return r;
    }
}

using Eq = Catch::Vector::EqualsMatcher<TestToken>;

TEST_CASE("lexer", "[lexer]")
{
    SECTION("empty")
    {
        const auto equal_empty = Equals(std::vector<TestToken>{});
        CHECK_THAT(C(GetAllTokensInFile(S(""))), equal_empty);
        CHECK_THAT(C(GetAllTokensInFile(S(" "))), equal_empty);
        CHECK_THAT(C(GetAllTokensInFile(S("\n\t\n"))), equal_empty);
        CHECK_THAT(C(GetAllTokensInFile(S("  /* all dogs are nice */  "))), equal_empty);
        CHECK_THAT(C(GetAllTokensInFile(S("  // cats are cool"))), equal_empty);
    }

    SECTION("operators")
    {
        CHECK_THAT(C(GetAllTokensInFile(S("(..)"))), Eq({TokenType::OpenParen, TokenType::DotDot, TokenType::CloseParen}));
        CHECK_THAT(C(GetAllTokensInFile(S("[if]"))), Eq({TokenType::OpenBracket, TokenType::KeywordIf, TokenType::CloseBracket}));
        CHECK_THAT(C(GetAllTokensInFile(S("{.}"))), Eq({TokenType::BeginBrace, TokenType::Dot, TokenType::EndBrace}));
    }
}
