#ifndef FEL_AST_H
#define FEL_AST_H

#include <vector>
#include <memory>

namespace fel
{
    struct Statement;
    struct Value;

    // ========================================================================

    using ValuePtr = std::shared_ptr<Value>;
    using ValueList = std::vector<ValuePtr>;

    using StatementPtr = std::shared_ptr<Statement>;

    // ========================================================================

    struct Callable
    {
        virtual ~Callable() = default;
    };

    struct Value
    {
        virtual ~Value() = default;
    };

    struct Statement
    {
        virtual ~Statement() = default;
    };

    // ========================================================================

    struct ValueBool : public Value
    {
        ValueBool(bool a_value);

        bool value;
    };

    struct ValueInt : public Value
    {
        ValueInt(int a_value);

        int value;
    };

    struct ValueNumber : public Value
    {
        ValueNumber(float a_value);

        float value;
    };

    struct ValueString : public Value
    {
        ValueString(const std::string& a_value);

        std::string value;
    };

    struct ValueNull : public Value
    {
    };

    struct ValueIdent : public Value
    {
        ValueIdent(const std::string& a_name);

        std::string name;
    };

    struct ValueFunctionDefinition : public Value
    {
        ValueFunctionDefinition(StatementPtr a_statements);
        
        StatementPtr statements;
    };

    struct ValueCallFunction : public Value
    {
        ValueCallFunction(ValuePtr a_function, ValueList a_arguments);
        
        ValuePtr function;
        ValueList arguments;
    };

    struct ValueCallArray : public Value
    {
        ValueCallArray(ValuePtr a_function, ValueList a_arguments);
        
        ValuePtr function;
        ValueList arguments;
    };

    struct ValueDotAccess : public Value
    {
        ValueDotAccess(ValuePtr a_parent, ValuePtr a_child);

        ValuePtr parent;
        ValuePtr child;
    };

    // ========================================================================

    struct StatementNull : public Statement
    {
    };

    struct StatementDeclaration : public Statement
    {
        StatementDeclaration(const std::string& a_name, ValuePtr a_value);

        std::string name;
        ValuePtr value;
    };

    struct StatementList : public Statement
    {
        StatementList() = default;

        std::vector<StatementPtr> statements;
    };

    struct StatementReturn : public Statement
    {
    };

    struct StatementReturnValue : public Statement
    {
        StatementReturnValue(ValuePtr a_value);

        ValuePtr value;
    };

    struct StatementConditionIf : public Statement
    {
        StatementConditionIf(ValuePtr a_condition, StatementPtr a_if_true);

        ValuePtr condition;
        StatementPtr if_true;
    };

    struct StatementValue : public Statement
    {
        StatementValue(ValuePtr a_value);

        ValuePtr value;
    };

    struct StatementAssign : public Statement
    {
        StatementAssign(ValuePtr a_lhs, ValuePtr a_rhs);

        ValuePtr lhs;
        ValuePtr rhs;
    };
}

#endif  // FEL_AST_H
