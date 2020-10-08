#include "fel/ast_printer.h"

#include <vector>
#include <sstream>

namespace fel
{
    void AstPrinter::Visit(Expression* expression)
    {
        expression->Visit(this);
    }


    std::string
    Parenthesize
    (
        const std::string& name,
        const std::vector<std::shared_ptr<Expression>>& expressions
    )
    {
        std::ostringstream ss;

        ss << "(" << name;

        for(const auto& exp: expressions)
        {
            AstPrinter visitor;
            ss << " " << exp->Visit(&visitor);
        }

        ss << ")";

        return ss.str();
    }


    std::string AstPrinter::Visit(BinaryExpression* exp)
    {
        return Parenthesize
        (
            exp->op.lexeme,
            {
                exp->left,
                exp->right
            }
        );
    }


    std::string AstPrinter::Visit(GroupingExpression* exp)
    {
        return Parenthesize("group", {exp->expression});
    }


    std::string AstPrinter::Visit(LiteralExpression* exp)
    {
        if(exp->value == nullptr)
        {
            return "null";
        }
        else
        {
            return exp->value->ToString();
        }
        
    }


    std::string AstPrinter::Visit(UnaryExpression* exp)
    {
        return Parenthesize(exp->op.lexeme, {exp->right});
    }


}
