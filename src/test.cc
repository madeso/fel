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

int argn(int args, int n)
{
  assert(args > 0);
  assert(n >=0 && n < args);
  const auto r = -args + n;
  return r;
}

TEST_CASE("argn", "[fel]")
{
  CHECK(-1 == argn(1, 0));
  CHECK(-2 == argn(2, 0));
}

TEST_CASE("basic", "[fel]")
{
  fel::Fel f;
  fel::Log log;
  const auto no_entries = std::vector<fel::LogEntry>{};

  std::vector<std::string> output;
  f.SetFunction("print", [&output](int args, fel::State* state) -> int {
      std::stringstream ss;
      for(int i=0; i<args; ++i)
      {
        ss << state->as_string(argn(args, i));
      }
      output.push_back(ss.str());
      return 0;
  });
  f.SetFunction("abc", [](int args, fel::State* state) -> int {
      state->Push("abc");
      return 1;
  });
  f.SetFunction("join", [](int args, fel::State* state) -> int {
      std::stringstream ss;
      for(int i=0; i<args; ++i)
      {
        ss << state->as_string(argn(args, i));
      }
      state->Push(ss.str());
      return 1;
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

  SECTION("calling print with more than 1 argument")
  {
    f.LoadAndRunString("print('a', 'b', 'c');", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"abc"};
    CHECK(output == expected);
  }

  SECTION("printing result of abc function")
  {
    f.LoadAndRunString("print(abc());", "abc.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"abc"};
    CHECK(output == expected);
  }

  SECTION("calling join with 2 arguments into print")
  {
    f.LoadAndRunString("print(join('hello', 'world'));", "filename.fel", &log);
    CHECK(log.entries == no_entries);

    const auto expected = std::vector<std::string>{"hello world"};
    CHECK(output == expected);
  }

}

