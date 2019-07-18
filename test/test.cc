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

  f.SetFunction("a", [&] (fel::State*) {result += "a";});
  f.SetFunction("b", [&] (fel::State*) {result += "b";});
  f.SetFunction("c", [&] (fel::State* s)
      {
        if(s->arguments>0) result += std::string(s->GetArg(0), 'c');
      }
    );
  f.SetFunction("d", [&] (fel::State* s)
      {
        for(int i=0; i<s->arguments; i+=1)
        {
          std::ostringstream ss;
          ss << s->GetArg(i);
          result += ss.str();
        }
      }
    );

  SECTION("call missing()")
  {
    f.LoadAndRunString("missing();", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
    REQUIRE(log.entries.size() == 1);
    const auto& e = log.entries[0];
    CHECK(e.file == "filename.fel");
    CHECK(e.location.line == 1);
    CHECK(e.location.column == 0);
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

  SECTION("call c()")
  {
    f.LoadAndRunString("c();", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "");
  }

  SECTION("call c(5)")
  {
    f.LoadAndRunString("c(5);", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "ccccc");
  }

  SECTION("call c(3, 2)")
  {
    f.LoadAndRunString("c(3, 2);", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "ccc");
  }

  SECTION("call d(3, 2)")
  {
    f.LoadAndRunString("d(3, 2, 42);", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "3242");
  }

}

