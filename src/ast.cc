#include "ast.h"

#include <sstream>

namespace fel
{
    struct Printer
    {
        std::ostringstream& stream;
        Printer(std::ostringstream& s) : stream(s) {}

        void Newline()
        {
            stream << "\n";
        }

        int current_indent = 0;
        void Indent()
        {
            for(int i=0; i<current_indent; i+=1)
            {
                stream << "    ";
            }
        }
        void BeginScope() { current_indent += 1; }
        void EndScope() { current_indent -= 1; }
    };

    void Print(Printer* printer, Statement* statement, bool root=false);
    void Print(Printer* printer, Value* statement);

    struct PrintValueVisitor : public ValueVisitor
    {
        Printer* print = nullptr;

        void Visit(ValueBool* value) override { print->stream << (value->value ? "true" : "false"); }
        void Visit(ValueInt* value) override { print->stream << value->value; }
        void Visit(ValueNumber* value) override { print->stream << value->value; }
        void Visit(ValueString* value) override { print->stream << '"' << value->value << '"'; }
        void Visit(ValueNull* ) override { print->stream << "null"; }
        void Visit(ValueIdent* value) override { print->stream << value->name; }
        void Visit(ValueFunctionDefinition* value) override
        {
            print->stream << "fun() ";
            Print(print, value->statements.get());
        }
        void Visit(ValueCallFunction* value) override
        {
            Print(print, value->function.get());
            print->stream << "(";
            bool first = true;
            for(auto v: value->arguments)
            {
                if(first) first = false;
                else print->stream << ", ";
                Print(print, v.get());
            }
            print->stream << ")";
        }
        void Visit(ValueCallArray* value) override
        {
            Print(print, value->function.get());
            print->stream << "[";
            bool first = true;
            for(auto v: value->arguments)
            {
                if(first) first = false;
                else print->stream << ", ";
                Print(print, v.get());
            }
            print->stream << "]";
        }
        void Visit(ValueDotAccess* value) override
        {
            Print(print, value->parent.get());
            print->stream << ".";
            Print(print, value->child.get());
        }
    };

    // ========================================================================

    struct PrintStatementVisitor : public StatementVisitor
    {
        Printer* print = nullptr;
        bool root = false;

        void Visit(StatementNull* ) override
        {
            print->stream << ";";
        }
        void Visit(StatementDeclaration* statement) override
        {
            print->stream << "var " << statement->name << " = ";
            Print(print, statement->value.get());
            print->stream << ";";
        }
        void Visit(StatementList* statement) override
        {
            if(!root)
            {
                print->Newline();
                print->Indent();
                print->stream << "{";
                print->Newline();
                print->BeginScope();
            }
            for(auto s: statement->statements)
            {
                print->Indent();
                Print(print, s.get());
                print->Newline();
            }
            if(!root)
            {
                print->EndScope();
                print->Indent();
                print->stream << "}";
            }
        }
        void Visit(StatementReturn* ) override
        {
            print->stream << "return;";
        }
        void Visit(StatementReturnValue* statement) override
        {
            print->stream << "return ";
            Print(print, statement->value.get());
            print->stream << ";";
        }
        void Visit(StatementConditionIf* statement) override
        {
            print->stream << "if(";
            Print(print, statement->condition.get());
            print->stream << ") ";
            Print(print, statement->if_true.get());
        }
        void Visit(StatementValue* statement) override
        {
            Print(print, statement->value.get());
            print->stream << ";";
        }
        void Visit(StatementAssign* statement) override
        {
            Print(print, statement->lhs.get());
            print->stream << " = ";
            Print(print, statement->rhs.get());
            print->stream << ";";
        }
    };

    void Print(Printer* printer, Statement* statement, bool root)
    {
        auto visitor = PrintStatementVisitor{};
        visitor.print = printer;
        visitor.root = root;
        statement->Visit(&visitor);
    }

    void Print(Printer* printer, Value* statement)
    {
        auto visitor = PrintValueVisitor{};
        visitor.print = printer;
        statement->Visit(&visitor);
    }

    std::string PrintStatement(StatementPtr statement)
    {
        std::ostringstream ss;
        if(statement != nullptr)
        {
            auto printer = Printer{ss};
            Print(&printer, statement.get(), true);
        }
        return ss.str();
    }

    // ========================================================================

    ValueBool::ValueBool(bool a_value)                                                       : value(a_value) {}
    ValueInt::ValueInt(int a_value)                                                          : value(a_value) {}
    ValueNumber::ValueNumber(float a_value)                                                  : value(a_value) {}
    ValueString::ValueString(const std::string& a_value)                                     : value(a_value) {}
    ValueIdent::ValueIdent(const std::string& a_name)                                        : name(a_name) {}
    ValueFunctionDefinition::ValueFunctionDefinition(StatementPtr a_statements)              : statements(a_statements) {}
    ValueCallFunction::ValueCallFunction(ValuePtr a_function, ValueList a_arguments)         : function(a_function), arguments(a_arguments) {}
    ValueCallArray::ValueCallArray(ValuePtr a_function, ValueList a_arguments)               : function(a_function), arguments(a_arguments) {}
    ValueDotAccess::ValueDotAccess(ValuePtr a_parent, ValuePtr a_child)                      : parent(a_parent), child(a_child) {}
    StatementDeclaration::StatementDeclaration(const std::string& a_name, ValuePtr a_value)  : name(a_name), value(a_value) {}
    StatementReturnValue::StatementReturnValue(ValuePtr a_value)                             : value(a_value) {}
    StatementConditionIf::StatementConditionIf(ValuePtr a_condition, StatementPtr a_if_true) : condition(a_condition), if_true(a_if_true) {}
    StatementValue::StatementValue(ValuePtr a_value)                                         : value(a_value) {}
    StatementAssign::StatementAssign(ValuePtr a_lhs, ValuePtr a_rhs)                         : lhs(a_lhs), rhs(a_rhs) {}

    // ========================================================================

    void ValueBool::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueInt::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueNumber::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueString::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueNull::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueIdent::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueFunctionDefinition::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueCallFunction::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueCallArray::Visit(ValueVisitor* vis) { vis->Visit(this); }
    void ValueDotAccess::Visit(ValueVisitor* vis) { vis->Visit(this); }

    void StatementNull::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementDeclaration::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementList::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementReturn::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementReturnValue::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementConditionIf::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementValue::Visit(StatementVisitor* vis) { vis->Visit(this); }
    void StatementAssign::Visit(StatementVisitor* vis) { vis->Visit(this); }
}
