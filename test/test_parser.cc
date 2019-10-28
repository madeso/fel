#include "catch.hpp"

#include <sstream>
#include <iostream>
#include "parser.h"
#include "lexer.h"
#include "file.h"
#include "log.h"

using namespace fel;

namespace
{
    bool Parse(const std::string& source, Log* log)
    {
        auto file = File{"source", source};
        auto reader = LexerReader{file, log};
        return fel::Parse(&reader, log);
    }
}

TEST_CASE("parser", "[parser]")
{
    Log log;
    SECTION("empty")
    {
        INFO(log);
        CHECK(Parse("", &log));
    }

    SECTION("function call")
    {
        INFO(log);
        CHECK(Parse("dog();", &log));
    }

    SECTION("assignment")
    {
        INFO(log);
        CHECK(Parse("cat = awesome;", &log));
    }

    SECTION("bad assignment")
    {
        INFO(log);
        CHECK_FALSE(Parse("a =", &log));
    }

    SECTION("bad function call")
    {
        INFO(log);
        CHECK_FALSE(Parse("b(", &log));
    }
}
