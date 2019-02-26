#include "bytecode.h"

std::ostream& operator<<(std::ostream& s, const Operation o)
{ 
  switch(o)
  {
    case Operation::PushConstantString: s << "PushConstantString"; break;
    case Operation::Pop: s << "Pop"; break;
    case Operation::CallFunctionDiscardReturn: s << "CallFunction_discard"; break;
    case Operation::CallFunctionWithReturn: s << "CallFunction_ret"; break;
  }

  return s;
}

Bytecode::Bytecode(const Operation o, int arg)
  : operation(o)
    , argument(arg)
{
}

bool operator==(const Bytecode& lhs, const Bytecode& rhs)
{
  return lhs.operation == rhs.operation
    && lhs.argument == rhs.argument;
}

std::ostream& operator<<(std::ostream& s, const Bytecode& b)
{
  s << "(" << b.operation << ", " << b.argument << ")";
  return s;
}

Compiler::Compiler(CompiledCode* cc)
  : code(cc)
{
}

Compiler& Compiler::PushString(const std::string& str)
{
  int index = code->strings.size();
  code->codes.push_back(Bytecode{Operation::PushConstantString, index});
  code->strings.emplace_back(str);
  return *this;
}

Compiler& Compiler::Pop(int arg)
{
  code->codes.push_back(Bytecode{Operation::Pop, arg});
  return *this;
}

Compiler& Compiler::CallFunctionDiscardReturn(const std::string& name, int arg)
{
  PushString(name);
  code->codes.push_back(Bytecode{Operation::CallFunctionDiscardReturn, arg});
  return *this;
}

Compiler& Compiler::CallFunctionWithReturn(const std::string& name, int arg)
{
  PushString(name);
  code->codes.push_back(Bytecode{Operation::CallFunctionWithReturn, arg});
  return *this;
}

