#include "catch.hpp"

#include <sstream>
#include "lexer.h"
#include "file.h"

using namespace fel;

namespace
{
    File S(const std::string& source)
    {
        return {"source", source};
    }
}

TEST_CASE("lexer", "[lexer]")
{
    SECTION("empty")
    {
        const auto equal_empty = Catch::Matchers::Equals(std::vector<Token>{});
        CHECK_THAT(GetAllTokensInFile(S("")), equal_empty);
        CHECK_THAT(GetAllTokensInFile(S(" ")), equal_empty);
        CHECK_THAT(GetAllTokensInFile(S("\n\t\n")), equal_empty);
        CHECK_THAT(GetAllTokensInFile(S("  /* all dogs are nice */  ")), equal_empty);
        CHECK_THAT(GetAllTokensInFile(S("  // cats are cool")), equal_empty);
    }
}
