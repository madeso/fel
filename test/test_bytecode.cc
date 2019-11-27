#include "catch.hpp"
#include "bytecode.h"

using namespace fel;

TEST_CASE("bytecode", "[bytecode]")
{
    CHECK(Command::FromInt(0).instruction == Instruction::Nop);
}
