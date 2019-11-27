#include "bytecode.h"

#include <cassert>

namespace fel
{
    namespace
    {
        using U = uint32_t;
        constexpr U INSTRUCTION_MASK = 0b111111;
        constexpr U INSTRUCTION_STEP = 26;
        constexpr U ARG_MASK = ~(INSTRUCTION_MASK << INSTRUCTION_STEP);
    }

    std::uint32_t Command::ToInt() const
    {
        assert((static_cast<U>(instruction) & INSTRUCTION_MASK) == static_cast<U>(instruction));
        assert((arg0 & ARG_MASK) == arg0);
        return (static_cast<U>(instruction) << INSTRUCTION_STEP) | arg0;
    }

    Command Command::FromInt(std::uint32_t t)
    {
        Command c;
        c.instruction = static_cast<Instruction>((t >> INSTRUCTION_STEP) & INSTRUCTION_MASK);
        c.arg0 = t & ARG_MASK;
        return c;
    }
}
