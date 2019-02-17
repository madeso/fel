#include "catch.hpp"

#include "fel.h"

TEST_CASE("empty", "[fel]" )
{
  fel::Fel f;
  fel::Log log;
  f.LoadAndRunString("", "filename.fel", &log);

  CHECK(log.entries.empty());
}

TEST_CASE("basic", "[fel]")
{
  fel::Fel f;
  fel::Log log;
  const auto no_entries = std::vector<fel::LogEntry>{};

  std::vector<std::string> output;
  f.SetFunction("print", [&output](const std::string& arg) { output.push_back(arg); } );

  SECTION("print")
  {
    f.LoadAndRunString("print(\"hello world\");", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello world"};
    CHECK(output == expected);
  }
}

