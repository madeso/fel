#include "catch.hpp"

#include <sstream>
#include "fel.h"

TEST_CASE("empty", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("", "filename.fel", &log);

  CHECK(log);
}

TEST_CASE("syntax error", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("dog", "filename.fel", &log);

  CHECK_FALSE(log);
}

TEST_CASE("call function", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  std::string result;

  f.SetFunction("a", [&] {result += "a";});
  f.SetFunction("b", [&] {result += "b";});

  SECTION("call missing()")
  {
    f.LoadAndRunString("missing();", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
    REQUIRE(log.entries.size() == 1);
    const auto& e = log.entries[0];
    CHECK(e.file == "filename.fel");
    CHECK(e.line == 1);
    CHECK(e.column == 0);
    CHECK(e.message == "missing is not a function");
  }

  SECTION("call a()")
  {
    f.LoadAndRunString("a();", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "a");
  }

  SECTION("call a() and b()")
  {
    f.LoadAndRunString("a(); b();", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "ab");
  }
}

