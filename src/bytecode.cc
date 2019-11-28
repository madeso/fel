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

    std::string_view ToString(Instruction inst)
    {
        switch(inst)
        {
            case Instruction::Nop: return "Nop"; 
            case Instruction::PushConstInt: return "PushConstInt"; 
            case Instruction::PushConstNumber: return "PushConstNumber"; 
            case Instruction::PushConstNull: return "PushConstNull"; 
            case Instruction::PushConstBool: return "PushConstBool"; 
            case Instruction::PushConstString: return "PushConstString"; 
            case Instruction::PushLocalFunction: return "PushLocalFunction"; 
            case Instruction::PushGlobal: return "PushGlobal"; 
            case Instruction::Pop: return "Pop"; 
            case Instruction::Goto: return "Goto"; 
            case Instruction::IfTrueGoto: return "IfTrueGoto"; 
            default: return "<unknown>";
        }
    }

    Command::Command(Instruction inst, unsigned int a0)
        : instruction(inst)
        , arg0(a0)
    {
        assert((arg0 & ARG_MASK) == arg0);
    }

    std::uint32_t Command::ToInt() const
    {
        assert((static_cast<U>(instruction) & INSTRUCTION_MASK) == static_cast<U>(instruction));
        assert((arg0 & ARG_MASK) == arg0);
        return (static_cast<U>(instruction) << INSTRUCTION_STEP) | arg0;
    }

    Command Command::FromInt(std::uint32_t t)
    {
        const auto instruction = static_cast<Instruction>((t >> INSTRUCTION_STEP) & INSTRUCTION_MASK);
        const auto arg0 = t & ARG_MASK;
        return Command{instruction, arg0};
    }
}
