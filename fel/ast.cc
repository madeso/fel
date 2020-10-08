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


    GroupingExpression::GroupingExpression(std::shared_ptr<Expression> e)
        : expression(e)
    {
    }


    LiteralExpression::LiteralExpression(std::shared_ptr<Object> v)
        : value(v)
    {
    }


    UnaryExpression::UnaryExpression
    (
        const Token& o,
        std::shared_ptr<Expression> r
    )
        : op(o)
        , right(r)
    {
    }
}