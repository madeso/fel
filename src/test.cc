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


TEST_CASE("call missing function", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("func();", "filename.fel", &log);

  CHECK_FALSE(log);
}

TEST_CASE("call function", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("func();", "filename.fel", &log);

  CHECK(log);
}

