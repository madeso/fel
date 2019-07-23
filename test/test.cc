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

  f.SetFunction("junk", [&] (fel::State*) {result += "!"; return 0;});
  f.SetFunction("a", [&] (fel::State*) {result += "a"; return 0;});
  f.SetFunction("b", [&] (fel::State*) {result += "b"; return 0;});
  f.SetFunction("c", [&] (fel::State* s)
      {
        if(s->arguments>0)
        {
        auto arg = s->GetArg(0);
        auto* argi = arg->AsInt();
          if(argi)
          {
            result += std::string(argi->value, 'c');
          }
        }
        return 0;
      }
    );
  f.SetFunction("d", [&] (fel::State* s)
      {
        for(int i=0; i<s->arguments; i+=1)
        {
          result += s->GetArg(i)->GetStringRepresentation();
        }
        return 0;
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

  SECTION("call junk()")
  {
    f.LoadAndRunString("junk();", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "!");
  }

  SECTION("call junk(1 2)")
  {
    f.LoadAndRunString("junk(1 2);", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
    REQUIRE(log.entries.size() == 1);
    const auto& e = log.entries[0];
    CHECK(e.file == "filename.fel");
    CHECK(e.location.line == 1);
    CHECK(e.location.column == 8);
    CHECK(e.message == "Error: Expected ',' but found 2");
  }

  SECTION("call junk(42,)")
  {
    f.LoadAndRunString("junk(42,);", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
    REQUIRE(log.entries.size() == 1);
    const auto& e = log.entries[0];
    CHECK(e.file == "filename.fel");
    CHECK(e.location.line == 1);
    CHECK(e.location.column == 8);
    CHECK(e.message == "Found ) while looking for rvalue");
  }

}

TEST_CASE("operator +", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  std::string result;
  f.SetFunction("p", [&] (fel::State* s)
      {
        for(int i=0; i<s->arguments; i+=1)
        {
          result += s->GetArg(i)->GetStringRepresentation();
        }
        return 0;
      }
    );
  
  SECTION("print(1+2+3)")
  {
    f.LoadAndRunString("p(1+2+3);", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "6");
  }

  SECTION("print(1+b)")
  {
    f.LoadAndRunString("p(1+'b');", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
  }

  SECTION("print(a+2)")
  {
    f.LoadAndRunString("p('a'+2);", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
  }

  SECTION("print(a+b)")
  {
    f.LoadAndRunString("p('a'+'b');", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "ab");
  }
}

