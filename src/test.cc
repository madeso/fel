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

TEST_CASE("unable to fun", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("dog", "filename.fel", &log);

  CHECK_FALSE(log);
}

