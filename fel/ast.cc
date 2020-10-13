#include "fel/ast.h"

namespace fel
{
    BinaryExpression::BinaryExpression
    (
        std::shared_ptr<Expression> l,
        const Token& o,
        std::shared_ptr<Expression> r
    )
        : left(l)
        , op(o)
        , right(r)
    {
    }


    Where
    BinaryExpression::GetLocation()
    {
        return left->GetLocation();
    }


    std::string
    BinaryExpression::Visit(ExpressionVisitorString* visitor)
    {
        return visitor->Visit(this);
    }

    std::shared_ptr<Object>
    BinaryExpression::Visit(ExpressionVisitorObject* visitor)
    {
        return visitor->Visit(this);
    }


    // ------------------------------------------------------------------------


    GroupingExpression::GroupingExpression(std::shared_ptr<Expression> e)
        : expression(e)
    {
    }


    Where
    GroupingExpression::GetLocation()
    {
        return expression->GetLocation();
    }


    std::string
    GroupingExpression::Visit(ExpressionVisitorString* visitor)
    {
        return visitor->Visit(this);
    }


    std::shared_ptr<Object>
    GroupingExpression::Visit(ExpressionVisitorObject* visitor)
    {
        return visitor->Visit(this);
    }


    // ------------------------------------------------------------------------


    LiteralExpression::LiteralExpression(std::shared_ptr<Object> v, const Where& w)
        : value(v)
        , where(w)
    {
    }


    Where
    LiteralExpression::GetLocation()
    {
        return where;
    }


    std::string
    LiteralExpression::Visit(ExpressionVisitorString* visitor)
    {
        return visitor->Visit(this);
    }

    std::shared_ptr<Object>
    LiteralExpression::Visit(ExpressionVisitorObject* visitor)
    {
        return visitor->Visit(this);
    }


    // ------------------------------------------------------------------------


    UnaryExpression::UnaryExpression
    (
        const Token& o,
        std::shared_ptr<Expression> r
    )
        : op(o)
        , right(r)
    {
    }


    Where
    UnaryExpression::GetLocation()
    {
        return op.where;
    }


    std::string
    UnaryExpression::Visit(ExpressionVisitorString* visitor)
    {
        return visitor->Visit(this);
    }

    std::shared_ptr<Object>
    UnaryExpression::Visit(ExpressionVisitorObject* visitor)
    {
        return visitor->Visit(this);
    }
}