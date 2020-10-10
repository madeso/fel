#ifndef FEL_AST_PRINTER_H
#define FEL_AST_PRINTER_H

#include "fel/ast.h"

namespace fel
{

    struct AstPrinter : public ExpressionVisitorString
    {
        std::string Visit(Expression* expression);

        std::string Visit(BinaryExpression* exp) override;
        std::string Visit(GroupingExpression* exp) override;
        std::string Visit(LiteralExpression* exp) override;
        std::string Visit(UnaryExpression* exp) override;
    };

}

#endif  // FEL_AST_PRINTER_H
