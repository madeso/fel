#ifndef FEL_AST_H
#define FEL_AST_H

#include <vector>
#include <memory>

#include <string>
#include <functional>

namespace fel
{
    struct Statement;
    struct Value;

    // ========================================================================

    using ValuePtr = std::shared_ptr<Value>;
    using ValueList = std::vector<ValuePtr>;

    using StatementPtr = std::shared_ptr<Statement>;

    std::string PrintStatement(StatementPtr statement);

    // ========================================================================
    
    struct ValueVisitor;
    struct StatementVisitor;

    // ========================================================================

    struct Value
    {
        virtual ~Value() = default;
        virtual void Visit(ValueVisitor* vis) = 0;
    };

    struct Statement
    {
        virtual ~Statement() = default;
        virtual void Visit(StatementVisitor* vis) = 0;
    };

    // ========================================================================

    struct ValueBool : public Value
    {
        ValueBool(bool a_value);
        void Visit(ValueVisitor* vis) override;

        bool value;
    };

    struct ValueInt : public Value
    {
        ValueInt(int a_value);
        void Visit(ValueVisitor* vis) override;

        int value;
    };

    struct ValueNumber : public Value
    {
        ValueNumber(float a_value);
        void Visit(ValueVisitor* vis) override;

        float value;
    };

    struct ValueString : public Value
    {
        ValueString(const std::string& a_value);
        void Visit(ValueVisitor* vis) override;

        std::string value;
    };

    struct ValueNull : public Value
    {
        void Visit(ValueVisitor* vis) override;
    };

    struct ValueIdent : public Value
    {
        ValueIdent(const std::string& a_name);
        void Visit(ValueVisitor* vis) override;

        std::string name;
    };

    struct ValueFunctionDefinition : public Value
    {
        ValueFunctionDefinition(StatementPtr a_statements);
        void Visit(ValueVisitor* vis) override;
        
        StatementPtr statements;
    };

    struct ValueCallFunction : public Value
    {
        ValueCallFunction(ValuePtr a_function, ValueList a_arguments);
        void Visit(ValueVisitor* vis) override;
        
        ValuePtr function;
        ValueList arguments;
    };

    struct ValueCallArray : public Value
    {
        ValueCallArray(ValuePtr a_function, ValueList a_arguments);
        void Visit(ValueVisitor* vis) override;
        
        ValuePtr function;
        ValueList arguments;
    };

    struct ValueDotAccess : public Value
    {
        ValueDotAccess(ValuePtr a_parent, ValuePtr a_child);
        void Visit(ValueVisitor* vis) override;

        ValuePtr parent;
        ValuePtr child;
    };

    // ========================================================================

    struct StatementNull : public Statement
    {
        void Visit(StatementVisitor* vis) override;
    };

    struct StatementDeclaration : public Statement
    {
        StatementDeclaration(const std::string& a_name, ValuePtr a_value);
        void Visit(StatementVisitor* vis) override;

        std::string name;
        ValuePtr value;
    };

    struct StatementList : public Statement
    {
        StatementList() = default;
        void Visit(StatementVisitor* vis) override;

        std::vector<StatementPtr> statements;
    };

    struct StatementReturn : public Statement
    {
        void Visit(StatementVisitor* vis) override;
    };

    struct StatementReturnValue : public Statement
    {
        StatementReturnValue(ValuePtr a_value);
        void Visit(StatementVisitor* vis) override;

        ValuePtr value;
    };

    struct StatementConditionIf : public Statement
    {
        StatementConditionIf(ValuePtr a_condition, StatementPtr a_if_true);
        void Visit(StatementVisitor* vis) override;

        ValuePtr condition;
        StatementPtr if_true;
    };

    struct StatementValue : public Statement
    {
        StatementValue(ValuePtr a_value);
        void Visit(StatementVisitor* vis) override;

        ValuePtr value;
    };

    struct StatementAssign : public Statement
    {
        StatementAssign(ValuePtr a_lhs, ValuePtr a_rhs);
        void Visit(StatementVisitor* vis) override;

        ValuePtr lhs;
        ValuePtr rhs;
    };

    // ========================================================================

    struct ValueVisitor
    {
        virtual ~ValueVisitor() = default;

        virtual void Visit(ValueBool* value) = 0;
        virtual void Visit(ValueInt* value) = 0;
        virtual void Visit(ValueNumber* value) = 0;
        virtual void Visit(ValueString* value) = 0;
        virtual void Visit(ValueNull* value) = 0;
        virtual void Visit(ValueIdent* value) = 0;
        virtual void Visit(ValueFunctionDefinition* value) = 0;
        virtual void Visit(ValueCallFunction* value) = 0;
        virtual void Visit(ValueCallArray* value) = 0;
        virtual void Visit(ValueDotAccess* value) = 0;
    };

    // ========================================================================

    struct StatementVisitor
    {
        virtual ~StatementVisitor() = default;

        virtual void Visit(StatementNull* statement) = 0;
        virtual void Visit(StatementDeclaration* statement) = 0;
        virtual void Visit(StatementList* statement) = 0;
        virtual void Visit(StatementReturn* statement) = 0;
        virtual void Visit(StatementReturnValue* statement) = 0;
        virtual void Visit(StatementConditionIf* statement) = 0;
        virtual void Visit(StatementValue* statement) = 0;
        virtual void Visit(StatementAssign* statement) = 0;
    };

    // ========================================================================
}

#endif  // FEL_AST_H
