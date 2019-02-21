#ifndef FEL_AST_H
#define FEL_AST_H

#include <memory>
#include <string>
#include <vector>

namespace fel
{
  struct Log;
}

struct StatementVisitor;
struct ValueVisitor;

struct Statement
{
  virtual ~Statement() {}
  virtual void Visit(StatementVisitor* vis) const = 0;
};

struct Value
{
  virtual ~Value() {}
  virtual void Visit(ValueVisitor* vis) const = 0;
};

struct StatementList
{
  std::vector<std::shared_ptr<Statement>> statements;
  void VisitAll(StatementVisitor* vis);
};

struct FunctionCall : public Statement
{
  FunctionCall(const std::string& n, std::shared_ptr<Value> v) : name(n), arguments(v) {}
  void Visit(StatementVisitor* vis) const override;
  std::string name;
  std::shared_ptr<Value> arguments;
};

struct StringValue : public Value
{
  explicit StringValue(const std::string& v) : value(v) {}
  void Visit(ValueVisitor* vis) const override;
  std::string value;
};

struct StatementVisitor
{
  virtual void OnFunctionCall(const FunctionCall& fc) = 0;
};

struct ValueVisitor
{
  virtual void OnString(const StringValue& str) = 0;
};

void StringToAst(const std::string& str, const std::string& filename, fel::Log* log, StatementList* program);

#endif // FEL_AST_H
