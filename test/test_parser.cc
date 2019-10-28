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
    SECTION("ok")
    {
        SECTION("empty")
        {
            INFO(log);
            CHECK(Parse("", &log));
        }

        SECTION("function call 0")
        {
            INFO(log);
            CHECK(Parse("dog();", &log));
        }

        SECTION("function call 1")
        {
            INFO(log);
            CHECK(Parse("dog(42);", &log));
        }

        SECTION("function call 2")
        {
            INFO(log);
            CHECK(Parse("dog('dog', 42);", &log));
        }

        SECTION("function call 3")
        {
            INFO(log);
            CHECK(Parse("doggy(dog, doogy, dog);", &log));
        }

        SECTION("assignment")
        {
            INFO(log);
            CHECK(Parse("cat = awesome;", &log));
        }
    }

    SECTION("bad")
    {
        SECTION("bad assignment")
        {
            INFO(log);
            CHECK_FALSE(Parse("a =", &log));
        }

        SECTION("bad function call 1")
        {
            INFO(log);
            CHECK_FALSE(Parse("b(", &log));
        }

        SECTION("bad function call 2")
        {
            INFO(log);
            CHECK_FALSE(Parse("b(c,", &log));
        }

        SECTION("bad function call 3")
        {
            INFO(log);
            CHECK_FALSE(Parse("b(3,);", &log));
        }

        SECTION("bad function call 4")
        {
            INFO(log);
            CHECK_FALSE(Parse("b(,);", &log));
        }

        SECTION("bad function call 5")
        {
            INFO(log);
            CHECK_FALSE(Parse("b(3,,)", &log));
        }

        SECTION("bad function call 6")
        {
            INFO(log);
            CHECK_FALSE(Parse("b(3;4);", &log));
        }
    }
}
