#ifndef FEL_BYTECODE_H
#define FEL_BYTECODE_H

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>

#include "location.h"

namespace fel
{
    // 6 bit instruction: 0-63
    enum class Instruction : std::uint8_t
    {
        Nop,

        // load contants from respective array: arg0=index
        PushConstInt,
        PushConstNumber,
        PushConstNull,
        PushConstBool,
        PushConstString,

        // todo(Gustav): how do we handle structs/dictionaries/arrays in the bytecode?
        // push new dictionary
        // PushNewDict,

        // load function: arg0 = first instruction
        PushLocalFunction,

        // push the global struct
        PushGlobal,

        //arg0 - number of arguments?
        Pop,

        // todo(Gustav): How to handle arguments and return values in a dynamic language?
        // call function, arg0 the number of arguments
        // Call,
        // call local?
        // tail call optimization?

        // return: arg0 the number of arguments to return
        // Return,

        // goto: arg0 = the new location
        Goto,

        // if first item on stack is true, run goto, else nop, also drop first item on stack
        // arg0 = the jump location
        IfTrueGoto
    };

    std::string_view ToString(Instruction inst);

    // designed to fit in a 32 or 64 bit structure
    struct Command
    {
        Command() = delete;
        Command(Instruction inst, unsigned int a0);

        Instruction instruction;
        unsigned int arg0;
        // int arg1 = 0;
        // int arg2 = 0;
        
        // instruction takes 6 bits
        // depending on instruction:
        // either: arg1 at 26 (2 and 3 not used)
        //     or: arg1 at 18 and arg 2 at 8 (3 not used)
        //     or: arg1 and arg2 at 9 and arg 3 at 8

        std::uint32_t ToInt() const;
        static Command FromInt(std::uint32_t);
    };

    struct ByteCode
    {
        std::vector<std::string> constant_strings;
        std::vector<int> constant_ints;
        std::vector<float> constant_numbers;
        std::vector<int> instructions;
    };
}

#endif  // FEL_BYTECODE_H
