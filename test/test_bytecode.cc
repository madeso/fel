#include "catch.hpp"
#include "bytecode.h"

namespace fel
{
    bool
    operator==(const Command& lhs, const Command& rhs)
    {
        return lhs.instruction == rhs.instruction
            && lhs.arg0 == rhs.arg0
            ;
    }

    template<typename S>
    S&
    operator<<(S& s, const Command& c)
    {
        s << "[" << ToString(c.instruction) << " | " << c.arg0 << "]";
        return s;
    }
}


using namespace fel;


TEST_CASE("bytecode-nop", "[bytecode]")
{
    CHECK(Command::FromInt(0) == Command{Instruction::Nop, 0});
    CHECK(Command{Instruction::Nop, 0}.ToInt() == 0);
}

TEST_CASE("bytecode-between", "[bytecode]")
{
    const Command c = 
        GENERATE(as<Command>{},
            Command{Instruction::Nop, 0},
            Command{Instruction::Nop, 42},
            Command{Instruction::PushConstInt, 33},
            Command{Instruction::Goto, 156},
            Command{Instruction::IfTrueGoto, 75}
        );
    CHECK(Command::FromInt(c.ToInt()) == c);
}
