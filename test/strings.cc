#include "catch.hpp"

#include <sstream>
#include "fel.h"

TEST_CASE("string", "[string]" )
{
  fel::Fel f;
  fel::Log log;
  std::string result;

  f.SetFunction("f", [&] (fel::State* s)
      {
        for(int i=0; i<s->arguments; i+=1)
        {
          result += s->GetArg(i)->GetStringRepresentation();
        }
        return 0;
      }
    );

  SECTION("empty call")
  {
    f.LoadAndRunString("f();", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "");
  }

  SECTION("single quote")
  {
    f.LoadAndRunString("f('\"dog\"');", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "\"dog\"");
  }

  SECTION("double quote")
  {
    f.LoadAndRunString("f(\"'dog'\");", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "'dog'");
  }

  SECTION("esapes")
  {
    f.LoadAndRunString("f('\\\\ \\n \\r \\t');", "filename.fel", &log);
    CHECK(log);
    CHECK(result == "\\ \n \r \t");
  }

  SECTION("illegal escape")
  {
    f.LoadAndRunString("f('do\\g');", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
  }

  SECTION("invalid character in string")
  {
    f.LoadAndRunString("f('dog\t');", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
  }

  SECTION("unclosed string")
  {
    f.LoadAndRunString("f('dog);", "filename.fel", &log);
    CHECK_FALSE(log);
    CHECK(result == "");
  }

}

