#include "catch.hpp"

#include <sstream>
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

    bool operator==(const TestToken& lhs, const TestToken& rhs)
    {
        if(rhs.type && lhs.type != rhs.type)
        {
            return false;
        }

        if(rhs.text && lhs.text != rhs.text)
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
}
