#ifndef FEL_AST_H
#define FEL_AST_H

#include <memory>
#include <string>

#include "fel/lexer.h"
#include "fel/object.h"


namespace fel
{
    struct BinaryExpression;
    struct GroupingExpression;
    struct LiteralExpression;
    struct UnaryExpression;

    struct ExpressionVisitorString
    {
        virtual ~ExpressionVisitorString() = default;

        virtual std::string Visit(BinaryExpression* exp) = 0;
        virtual std::string Visit(GroupingExpression* exp) = 0;
        virtual std::string Visit(LiteralExpression* exp) = 0;
        virtual std::string Visit(UnaryExpression* exp) = 0;
    };

    struct Expression
    {
        virtual ~Expression() = default;

        virtual
        std::string Visit(ExpressionVisitorString* visitor) = 0;
    };


    struct BinaryExpression : public Expression
    {
        std::shared_ptr<Expression> left;
        Token op;
        std::shared_ptr<Expression> right;

        BinaryExpression
        (
            std::shared_ptr<Expression> l,
            const Token& o,
            std::shared_ptr<Expression> r
        );

        std::string Visit(ExpressionVisitorString* visitor) override
        {
            return visitor->Visit(this);
        }
    };

    struct GroupingExpression : public Expression
    {
        std::shared_ptr<Expression> expression;

        explicit GroupingExpression(std::shared_ptr<Expression> e);

        std::string Visit(ExpressionVisitorString* visitor) override
        {
            return visitor->Visit(this);
        }
    };

    struct LiteralExpression : public Expression
    {
        std::shared_ptr<Object> value;

        explicit LiteralExpression(std::shared_ptr<Object> v);

        std::string Visit(ExpressionVisitorString* visitor) override
        {
            return visitor->Visit(this);
        }
    };
    
    struct UnaryExpression : public Expression
    {
        Token op;
        std::shared_ptr<Expression> right;

        UnaryExpression
        (
            const Token& o,
            std::shared_ptr<Expression> r
        );

        std::string Visit(ExpressionVisitorString* visitor) override
        {
            return visitor->Visit(this);
        }
    };

}

#endif // FEL_AST_H
