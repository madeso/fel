#include "catch.hpp"

#include <sstream>
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
  f.SetFunction("print", [&output](int args, fel::State* state) {
      std::stringstream ss;
      for(int i=0; i<args; ++i)
      {
        ss << state->as_string(args-(i+1));
      }
      output.push_back(ss.str());
  });

  // double quoted string

  SECTION("print double quoted string")
  {
    f.LoadAndRunString("print(\"hello world\");", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello world"};
    CHECK(output == expected);
  }

  SECTION("print double quoted string with single quotes")
  {
    f.LoadAndRunString("print(\"hello 'world'\");", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello 'world'"};
    CHECK(output == expected);
  }

  SECTION("print double quoted string with double quotes")
  {
    f.LoadAndRunString("print(\"hello \\\"world\\\"\");", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello \"world\""};
    CHECK(output == expected);
  } 

  // single quoted string

  SECTION("print single quoted string")
  {
    f.LoadAndRunString("print('hello world');", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello world"};
    CHECK(output == expected);
  } 

  SECTION("print single quoted string with single quotes")
  {
    f.LoadAndRunString("print('hello \\'world\\'');", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello 'world'"};
    CHECK(output == expected);
  }

  SECTION("print single quoted string with double quotes")
  {
    f.LoadAndRunString("print('hello \"world\"');", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello \"world\""};
    CHECK(output == expected);
  } 
}

