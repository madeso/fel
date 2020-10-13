#include "fel/interpreter.h"

#include <cassert>

#include "fel/log.h"


namespace fel
{
    bool
    IsTruthy(std::shared_ptr<Object> obj)
    {
        // null is falsy
        if(obj == nullptr) { return false;}

        // false is falsy (true is true)
        if(obj->GetType() == ObjectType::Bool)
        {
            return static_cast<BoolObject*>(obj.get())->b;
        }

        // all others are true
        return true;
    }

    std::string
    TypeToString(std::shared_ptr<Object> object)
    {
        if(object == nullptr) { return "null";}
        switch(object->GetType())
        {
            // todo(Gustav): add the value here?
            case ObjectType::Bool: return "bool";
            case ObjectType::Int: return "int";
            case ObjectType::Number: return "number";
            case ObjectType::String: return "string";
            default:
                assert(false && "unhandled case");
                return "<internal_error>";
        }
    }


    Interpreter::Interpreter(Log* l)
        : log(l)
    {
    }


    std::shared_ptr<Object>
    BinaryHelper
    (
        Log* log,
        const Where& where,
        std::shared_ptr<Expression> lhs,
        std::shared_ptr<Expression> rhs,
        std::shared_ptr<Object> left,
        std::shared_ptr<Object> right,
        std::optional<std::function<int (int, int)>> int_function,
        std::optional<std::function<float (float, float)>> number_function,
        std::optional
        <
            std::function
            <
                std::optional<std::shared_ptr<Object>>
                (
                    std::shared_ptr<Object>,
                    std::shared_ptr<Object>
                )
            >
        > fallback_function = std::nullopt
    )
    {
        if(left == nullptr || right == nullptr)
        {
            log->AddError(where, log::Type::InvalidOperationOnNull);
            log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left)});
            log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right)});
            return nullptr;
        }

        if(int_function)
        {
            if(left->GetType() == ObjectType::Int && right->GetType() == ObjectType::Int)
            {
                return Object::FromInt((*int_function)
                (
                    static_cast<IntObject*>(left.get())->i,
                    static_cast<IntObject*>(right.get())->i
                ));
            }
        }

        const auto can_cast_to_number = [](std::shared_ptr<Object> object) -> bool
        {
            const auto type = object->GetType();
            return type == ObjectType::Int || type == ObjectType::Number;
        };

        const auto cast_to_number = [](std::shared_ptr<Object> object) -> float
        {
            const auto type = object->GetType();
            if(type == ObjectType::Int)
            {
                return static_cast<float>(static_cast<IntObject*>(object.get())->i);
            }
            if(type == ObjectType::Number)
            {
                return static_cast<FloatObject*>(object.get())->f;
            }
            assert(false && "invalid number type");
            return 0.0f;
        };

        if(number_function && can_cast_to_number(left) && can_cast_to_number(right))
        {
            return Object::FromFloat
            (
                (*number_function)(cast_to_number(left), can_cast_to_number(right))
            );
        }

        if(fallback_function)
        {
            auto fallback_value = (*fallback_function)(left, right);
            if(fallback_value)
            {
                return *fallback_value;
            }
        }

        log->AddError(where, log::Type::InvalidBinaryOperation);
        log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left)});
        log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right)});
        return nullptr;
    }


    std::shared_ptr<Object>
    Interpreter::Visit(BinaryExpression* exp)
    {
        auto left = Evaluate(exp->left);
        auto right = Evaluate(exp->right);

        switch(exp->op.type)
        {
            case TokenType::Minus: return BinaryHelper
            (
                log, exp->op.where,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs - rhs;},
                [](float lhs, float rhs) -> float { return lhs - rhs;}
            );
            case TokenType::Mult: return BinaryHelper
            (
                log, exp->op.where,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs * rhs;},
                [](float lhs, float rhs) -> float { return lhs * rhs;}
            );
            case TokenType::Div: return BinaryHelper
            (
                log, exp->op.where,
                exp->left, exp->right,
                left, right,
                nullptr,
                [](float lhs, float rhs) -> float { return lhs - rhs;}
            );
            case TokenType::Mod: return BinaryHelper
            (
                log, exp->op.where,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs % rhs;},
                nullptr
            );
            case TokenType::Plus: return BinaryHelper
            (
                log, exp->op.where,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs + rhs;},
                [](float lhs, float rhs) -> float { return lhs + rhs;},
                [](std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs) -> std::optional<std::shared_ptr<Object>>
                {
                    if(lhs->GetType() == ObjectType::String && rhs->GetType() == ObjectType::String)
                    {
                        return Object::FromString
                        (
                            static_cast<StringObject*>(lhs.get())->s
                            +
                            static_cast<StringObject*>(rhs.get())->s
                        );
                    }

                    return std::nullopt;
                }
            );

            default:
                log->AddError
                (
                    FEL_WHERE_HERE,
                    log::Type::InternalError,
                    {"unhandled case in binary switch"}
                );
                return nullptr;
        }
    }


    std::shared_ptr<Object>
    Interpreter::Visit(GroupingExpression* exp)
    {
        return Evaluate(exp->expression);
    }


    std::shared_ptr<Object>
    Interpreter::Visit(LiteralExpression* exp)
    {
        return exp->value;
    }


    std::shared_ptr<Object>
    Interpreter::Visit(UnaryExpression* exp)
    {
        auto right = Evaluate(exp->right);

        switch(exp->op.type)
        {
            case TokenType::Minus:
                switch(right->GetType())
                {
                    case ObjectType::Int: return Object::FromInt
                    (
                        static_cast<IntObject*>(right.get())->i * -1
                    );
                    case ObjectType::Number: return Object::FromFloat
                    (
                        static_cast<FloatObject*>(right.get())->f * -1
                    );

                    default:
                        log->AddError
                        (
                            FEL_WHERE_HERE,
                            log::Type::InternalError,
                            {"taking the negative of neither a number nor a int"}
                        );
                        return nullptr;
                }

            case TokenType::Not:
                return Object::FromBool(!IsTruthy(right));

            default:
                log->AddError
                (
                    FEL_WHERE_HERE,
                    log::Type::InternalError,
                    {"unhandled case in unary switch"}
                );
                return nullptr;
        }
    }


    std::shared_ptr<Object>
    Interpreter::Evaluate(std::shared_ptr<Expression> expression)
    {
        auto interpreter = Interpreter{log};
        return expression->Visit(&interpreter);
    }
}
