#ifndef FEL_AST_H
#define FEL_AST_H

#include <memory>
#include <string>
#include <vector>

struct Statement
{
  virtual ~Statement() {}
};

struct Value
{
  virtual ~Value() {}
};

struct StatementList
{
  std::vector<std::shared_ptr<Statement>> statements;
};

struct FunctionCall : public Statement
{
  FunctionCall(const std::string& n, std::shared_ptr<Value> v) : name(n), arguments(v) {}
  std::string name;
  std::shared_ptr<Value> arguments;
};

struct StringValue : public Value
{
  explicit StringValue(const std::string& v) : value(v) {}
  std::string value;
};

#endif // FEL_AST_H
