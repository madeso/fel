#include "ast.h"

namespace fel
{
    ValueBool::ValueBool(bool a_value)                                               : value(a_value) {}
    ValueInt::ValueInt(int a_value)                                                 : value(a_value) {}
    ValueNumber::ValueNumber(float a_value)                                            : value(a_value) {}
    ValueString::ValueString(const std::string& a_value)                               : value(a_value) {}
    ValueIdent::ValueIdent(const std::string& a_name)                                 : name(a_name) {}
    ValueFunctionDefinition::ValueFunctionDefinition(StatementPtr a_statements)                    : statements(a_statements) {}
    ValueCallFunction::ValueCallFunction(ValuePtr a_function, ValueList a_arguments)         : function(a_function), arguments(a_arguments) {}
    ValueCallArray::ValueCallArray(ValuePtr a_function, ValueList a_arguments)            : function(a_function), arguments(a_arguments) {}
    ValueDotAccess::ValueDotAccess(ValuePtr a_parent, ValuePtr a_child)                   : parent(a_parent), child(a_child) {}
    StatementDeclaration::StatementDeclaration(const std::string& a_name, ValuePtr a_value)     : name(a_name), value(a_value) {}
    StatementReturnValue::StatementReturnValue(ValuePtr a_value)                                : value(a_value) {}
    StatementConditionIf::StatementConditionIf(ValuePtr a_condition, StatementPtr a_if_true)    : condition(a_condition), if_true(a_if_true) {}
    StatementValue::StatementValue(ValuePtr a_value)                                      : value(a_value) {}
    StatementAssign::StatementAssign(ValuePtr a_lhs, ValuePtr a_rhs)                       : lhs(a_lhs), rhs(a_rhs) {}
}