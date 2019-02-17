#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "fel.h"

TEST_CASE("empty", "[fel]" ) {
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("", "filename.fel", &log);

  CHECK(log.entries.empty());
}
