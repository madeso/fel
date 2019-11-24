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
    fel::StatementPtr Parse(const std::string& source, Log* log, bool debug_parser)
    {
        auto file = File{"source", source};
        auto reader = LexerReader{file, log};
        return fel::Parse(&reader, log, debug_parser);
    }
}

#define RUN_INFO(x) const std::string code = "code: " #x; auto r = Parse(x, &log, debug_parser); const auto printed = PrintStatement(r); INFO(code);

TEST_CASE("parser", "[parser]")
{
    bool debug_parser = false;
    Log log;
    SECTION("ok")
    {
        SECTION("empty")
        {
            RUN_INFO("");
            CHECK(log);
            CHECK(printed == "");
        }

        SECTION("function call 0")
        {
            RUN_INFO("dog();");
            CHECK(log);
            CHECK(printed == "dog();\n");
        }

        SECTION("function call 1")
        {
            RUN_INFO("dog(42);");
            CHECK(log);
            CHECK(printed == "dog(42);\n");
        }

        SECTION("function call 2")
        {
            RUN_INFO("dog('dog', 42);");
            CHECK(log);
            CHECK(printed == "dog(\"dog\", 42);\n");
        }

        SECTION("function call 3")
        {
            RUN_INFO("doggy(dog, doogy, dog);");
            CHECK(log);
            CHECK(printed == "doggy(dog, doogy, dog);\n");
        }

        SECTION("2 function calls")
        {
            RUN_INFO("cat(); dog();");
            CHECK(log);
            CHECK(printed == "cat();\ndog();\n");
        }

        SECTION("assignment")
        {
            RUN_INFO("cat = awesome;");
            CHECK(log);
            CHECK(printed == "cat = awesome;\n");
        }

        SECTION("if no block")
        {
            RUN_INFO("if(dog) dog();");
            CHECK(log);
            CHECK(printed == "if(dog) dog();\n");
        }

        SECTION("if block")
        {
            RUN_INFO("if(cat) { cat(); }");
            CHECK(log);
            CHECK(printed == "if(cat) \n{\n    cat();\n}\n");
        }

        SECTION("if semi")
        {
            RUN_INFO("if(why);");
            CHECK(log);
            CHECK(printed == "if(why) ;\n");
        }

        SECTION("if empty block")
        {
            RUN_INFO("if(foo()) {}");
            CHECK(log);
            CHECK(printed == "if(foo()) \n{\n}\n");
        }

        SECTION("if empty block and func")
        {
            RUN_INFO("if(foo()) {} bar();");
            CHECK(log);
            CHECK(printed == "if(foo()) \n{\n}\nbar();\n");            
        }

        SECTION("just return")
        {
            RUN_INFO("return;");
            CHECK(log);
            CHECK(printed == "return;\n");
        }

        SECTION("return number")
        {
            RUN_INFO("return 42;");
            CHECK(log);
            CHECK(printed == "return 42;\n");
        }

        SECTION("if return")
        {
            RUN_INFO("if(foo()) {return cat;} return dog;");
            CHECK(log);
            CHECK(printed == "if(foo()) \n{\n    return cat;\n}\nreturn dog;\n");
        }

        SECTION("complex 1")
        {
            RUN_INFO("a.b.c[d,e].f(g,h) = i;");
            CHECK(log);
            CHECK(printed == "a.b.c[d, e].f(g, h) = i;\n");
        }

        SECTION("complex 2a")
        {
            RUN_INFO("fun() { return cat; } ().name = 'mittens';");
            CHECK(log);
            CHECK(printed == "fun() \n{\n    return cat;\n}().name = \"mittens\";\n");
        }
        SECTION("complex 2b")
        {
            RUN_INFO("fun() return dog;().name = 'sparky';");
            CHECK(log);
            CHECK(printed == "fun() return dog;().name = \"sparky\";\n");
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
