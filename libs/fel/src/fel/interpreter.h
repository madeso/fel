#ifndef FEL_INTERPRETER_H
#define FEL_INTERPRETER_H

#include "fel/ast.h"

namespace fel
{
    struct Log;

    struct Interpreter : public ExpressionVisitorObject
    {
        explicit Interpreter(Log* l);

        std::shared_ptr<Object>
        Visit(BinaryExpression* exp);

        std::shared_ptr<Object>
        Visit(GroupingExpression* exp);

        std::shared_ptr<Object>
        Visit(LiteralExpression* exp);

        std::shared_ptr<Object>
        Visit(UnaryExpression* exp);

        std::shared_ptr<Object>
        Evaluate(std::shared_ptr<Expression> expression);

        Log* log;
    };
}

#endif  // FEL_INTERPRETER_H
