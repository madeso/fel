#include "catch.hpp"

#include "bytecode.h"

TEST_CASE("bytecode", "[fel]")
{
  CompiledCode code;
  auto compiler = Compiler{&code};

  SECTION("push strings")
  {
    compiler
      .PushString("a")
      .PushString("b")
      ;
    const auto expected_code = std::vector<Bytecode>
    {
      Bytecode(Operation::PushConstantString, 0),
      Bytecode(Operation::PushConstantString, 1)
    };
    const auto expected_strings = std::vector<std::string>
    {
      "a",
      "b"
    };
    CHECK(code.codes == expected_code);
    CHECK(code.strings == expected_strings);
  }
}

