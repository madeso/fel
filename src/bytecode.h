#ifndef FEL_BYTECODE_H
#define FEL_BYTECODE_H

#include <vector>
#include <string>
#include <ostream>

enum class Operation
{
  // arg=string id
  PushConstantString,
  // arg=how many to pop
  Pop,
  // arg=how many arguments
  CallFunction
};

std::ostream& operator<<(std::ostream& s, const Operation o);

struct Bytecode
{
  Operation operation;
  int argument;

  Bytecode(const Operation o, int arg);
};

bool operator==(const Bytecode& lhs, const Bytecode& rhs);
std::ostream& operator<<(std::ostream& s, const Bytecode& b);

struct CompiledCode
{
  std::vector<Bytecode> codes;
  std::vector<std::string> strings;
};

struct Compiler
{
  CompiledCode* code;

  explicit Compiler(CompiledCode* cc);

  Compiler& PushString(const std::string& str);
  Compiler& Pop(int arg);
  Compiler& CallFunction(const std::string& name, int arg);
};

#endif  // FEL_BYTECODE_H
