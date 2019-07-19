#include "catch.hpp"

#include <sstream>
#include "fel.h"

TEST_CASE("comments", "[comments]" )
{
  fel::Fel f;
  fel::Log log;
  f.SetFunction("f", [&] (fel::State* s) { });

  SECTION("empty call")
  {
    f.LoadAndRunString("f();", "filename.fel", &log);
    CHECK(log);
  }

  SECTION("single line comment before")
  {
    f.LoadAndRunString("// comment \nf();", "filename.fel", &log);
    CHECK(log);
  }

  SECTION("single line comment after")
  {
    f.LoadAndRunString("f();// hello", "filename.fel", &log);
    CHECK(log);
  }

  SECTION("multi line comment")
  {
    f.LoadAndRunString("/*hello*/f();", "filename.fel", &log);
    CHECK(log);
  }

  SECTION("multi line comment all over the place")
  {
    f.LoadAndRunString("/*hello*/f/*is it*/(/*me you're*/)/*looking*/;/*for?*/", "filename.fel", &log);
    CHECK(log);
  }

  SECTION("nested multi line comment")
  {
    f.LoadAndRunString("/*nested/*comment*/here*/f();", "filename.fel", &log);
    CHECK(log);
  }

  SECTION("multi line comment not closed")
  {
    f.LoadAndRunString("f(); /*some comment*", "filename.fel", &log);
    CHECK_FALSE(log);
  }

}

