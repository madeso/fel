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

    struct ExpressionVisitorObject
    {
        virtual ~ExpressionVisitorObject() = default;

        virtual std::shared_ptr<Object> Visit(BinaryExpression* exp) = 0;
        virtual std::shared_ptr<Object> Visit(GroupingExpression* exp) = 0;
        virtual std::shared_ptr<Object> Visit(LiteralExpression* exp) = 0;
        virtual std::shared_ptr<Object> Visit(UnaryExpression* exp) = 0;
    };

    struct Expression
    {
        virtual ~Expression() = default;

        virtual Where
        GetLocation() = 0;

        virtual std::string
        Visit(ExpressionVisitorString* visitor) = 0;

        virtual std::shared_ptr<Object>
        Visit(ExpressionVisitorObject* visitor) = 0;
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

        Where
        GetLocation() override;

        std::string
        Visit(ExpressionVisitorString* visitor) override;

        std::shared_ptr<Object>
        Visit(ExpressionVisitorObject* visitor) override;
    };

    struct GroupingExpression : public Expression
    {
        std::shared_ptr<Expression> expression;

        explicit GroupingExpression(std::shared_ptr<Expression> e);

        Where
        GetLocation() override;

        std::string
        Visit(ExpressionVisitorString* visitor) override;

        std::shared_ptr<Object>
        Visit(ExpressionVisitorObject* visitor) override;
    };

    struct LiteralExpression : public Expression
    {
        std::shared_ptr<Object> value;
        Where where;

        LiteralExpression(std::shared_ptr<Object> v, const Where& w);

        Where
        GetLocation() override;

        std::string
        Visit(ExpressionVisitorString* visitor) override;

        std::shared_ptr<Object>
        Visit(ExpressionVisitorObject* visitor) override;
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

        Where
        GetLocation() override;

        std::string
        Visit(ExpressionVisitorString* visitor) override;

        std::shared_ptr<Object>
        Visit(ExpressionVisitorObject* visitor) override;
    };

}

#endif // FEL_AST_H
