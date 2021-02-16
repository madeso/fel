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


    bool CanCastToNumber(std::shared_ptr<Object> object)
    {
        const auto type = object->GetType();
        return type == ObjectType::Int || type == ObjectType::Number;
    }

    float CastToNumber(std::shared_ptr<Object> object)
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
    }


    std::shared_ptr<Object>
    BinaryHelper 
    (
        Log* log,
        const Token& op,
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
            log->AddError(op.where, log::Type::InvalidOperationOnNull, {op.lexeme});
            log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left), Stringify(left)});
            log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right), Stringify(right)});
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

        

        if(number_function && CanCastToNumber(left) && CanCastToNumber(right))
        {
            return Object::FromFloat
            (
                (*number_function)(CastToNumber(left), CanCastToNumber(right))
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

        log->AddError(op.where, log::Type::InvalidBinaryOperation, {op.lexeme});
        log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left), Stringify(left)});
        log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right), Stringify(right)});
        return nullptr;
    }


    std::shared_ptr<Object>
    CompareHelper 
    (
        Log* log,
        const Token& op,
        std::shared_ptr<Expression> lhs,
        std::shared_ptr<Expression> rhs,
        std::shared_ptr<Object> left,
        std::shared_ptr<Object> right,
        std::function<bool (float, float)> compare_function
    )
    {
        if(left == nullptr || right == nullptr)
        {
            log->AddError(op.where, log::Type::InvalidOperationOnNull, {op.lexeme});
            log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left), Stringify(left)});
            log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right), Stringify(right)});
            return nullptr;
        }

        
        if(CanCastToNumber(left) && CanCastToNumber(right))
        {
            return Object::FromBool
            (
                compare_function(CastToNumber(left), CanCastToNumber(right))
            );
        }

        // todo(Gustav): allow comparing of strings?

        log->AddError(op.where, log::Type::InvalidBinaryOperation, {op.lexeme});
        log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left), Stringify(left)});
        log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right), Stringify(right)});
        return nullptr;
    }


    std::shared_ptr<Object>
    EqualHelper 
    (
        Log* log,
        const Token& op,
        std::shared_ptr<Expression> lhs,
        std::shared_ptr<Expression> rhs,
        std::shared_ptr<Object> left,
        std::shared_ptr<Object> right,
        bool invert_result
    )
    {
        if(left == nullptr && right == nullptr) { return Object::FromBool(true); }
        if(left == nullptr || right == nullptr) { return Object::FromBool(false); }

        const auto lt = left->GetType();
        const auto rt = right->GetType();

        if(lt == rt)
        {
            if(lt == ObjectType::Bool)
            {
                return Object::FromBool
                (
                    invert_result
                    ||
                    (
                        static_cast<BoolObject*>(left.get())->b
                        ==
                        static_cast<BoolObject*>(right.get())->b
                    )
                );
            }
            else if(lt == ObjectType::Int)
            {
                return Object::FromBool
                (
                    invert_result
                    ||
                    (
                        static_cast<IntObject*>(left.get())->i
                        ==
                        static_cast<IntObject*>(right.get())->i
                    )
                );
            }
        }

        // todo(Gustav): allow comparing of strings?

        log->AddError(op.where, log::Type::InvalidBinaryOperation, {op.lexeme});
        log->AddError(lhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(left), Stringify(left)});
        log->AddError(rhs->GetLocation(), log::Type::ThisEvaluatesTo, {TypeToString(right), Stringify(right)});
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
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs - rhs;},
                [](float lhs, float rhs) -> float { return lhs - rhs;}
            );
            case TokenType::Mult: return BinaryHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs * rhs;},
                [](float lhs, float rhs) -> float { return lhs * rhs;}
            );
            case TokenType::Div: return BinaryHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                nullptr,
                [](float lhs, float rhs) -> float { return lhs - rhs;}
            );
            case TokenType::Mod: return BinaryHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](int lhs, int rhs) -> int {return lhs % rhs;},
                nullptr
            );
            case TokenType::Plus: return BinaryHelper
            (
                log, exp->op,
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
            case TokenType::Less: return CompareHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](float lhs, float rhs) -> bool { return lhs < rhs; }
            );
            case TokenType::LessEqual: return CompareHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](float lhs, float rhs) -> bool { return lhs <= rhs; }
            );
            case TokenType::Greater: return CompareHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](float lhs, float rhs) -> bool { return lhs > rhs; }
            );
            case TokenType::GreaterEqual: return CompareHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                [](float lhs, float rhs) -> bool { return lhs >= rhs; }
            );
            case TokenType::Equal: return EqualHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                false
            );
            case TokenType::NotEqual: return EqualHelper
            (
                log, exp->op,
                exp->left, exp->right,
                left, right,
                true
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
