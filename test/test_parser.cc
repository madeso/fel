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

#define RUN_INFO(x) Parse(x, &log); INFO("code: " x);

TEST_CASE("parser", "[parser]")
{
    Log log;
    SECTION("ok")
    {
        SECTION("empty")
        {
            RUN_INFO("");
            CHECK(log);
        }

        SECTION("function call 0")
        {
            RUN_INFO("dog();");
            CHECK(log);
        }

        SECTION("function call 1")
        {
            RUN_INFO("dog(42);");
            CHECK(log);
        }

        SECTION("function call 2")
        {
            RUN_INFO("dog('dog', 42);");
            CHECK(log);
        }

        SECTION("function call 3")
        {
            RUN_INFO("doggy(dog, doogy, dog);");
            CHECK(log);
        }

        SECTION("assignment")
        {
            RUN_INFO("cat = awesome;");
            CHECK(log);
        }

        SECTION("if no block")
        {
            RUN_INFO("if(dog) dog();");
            CHECK(log);
        }

        SECTION("if block")
        {
            RUN_INFO("if(cat) { cat(); }");
            CHECK(log);
        }

        SECTION("if semi")
        {
            RUN_INFO("if(why);");
            CHECK(log);
        }

        SECTION("if empty block")
        {
            RUN_INFO("if(foo()) {}");
            CHECK(log);
        }

        SECTION("complex 1")
        {
            RUN_INFO("a.b.c[d,e].f(g,h) = i;");
            CHECK(log);
        }

        SECTION("complex 2a")
        {
            RUN_INFO("fun() { return cat; } ().name = 'mittens';");
            CHECK(log);
        }
        SECTION("complex 2b")
        {
            RUN_INFO("fun() return dog;().name = 'sparky';");
            CHECK(log);
        }
    }

    SECTION("bad")
    {
        SECTION("bad assignment")
        {
            RUN_INFO("a =");
            CHECK_FALSE(log);
        }

        SECTION("bad function call 1")
        {
            RUN_INFO("b(");
            CHECK_FALSE(log);
        }

        SECTION("bad function call 2")
        {
            RUN_INFO("b(c,");
            CHECK_FALSE(log);
        }

        SECTION("bad function call 3")
        {
            RUN_INFO("b(3,);");
            CHECK_FALSE(log);
        }

        SECTION("bad function call 4")
        {
            RUN_INFO("b(,);");
            CHECK_FALSE(log);
        }

        SECTION("bad function call 5")
        {
            RUN_INFO("b(3,,)");
            CHECK_FALSE(log);
        }

        SECTION("bad function call 6")
        {
            RUN_INFO("b(3;4);");
            CHECK_FALSE(log);
        }
    }
}
