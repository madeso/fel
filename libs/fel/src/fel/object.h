#ifndef FEL_OBJECT_H
#define FEL_OBJECT_H

#include <string>
#include <memory>


namespace fel
{
    enum class ObjectType
    {
        Int, Number, Bool, String
    };


    struct Object
    {
        virtual ~Object() = default;

        virtual std::string ToString() = 0;
        virtual ObjectType GetType() = 0;

        static std::shared_ptr<Object> FromInt(int i);
        static std::shared_ptr<Object> FromFloat(float f);
        static std::shared_ptr<Object> FromBool(bool b);
        static std::shared_ptr<Object> FromString(const std::string& str);
    };


    std::string
    Stringify(std::shared_ptr<Object> object);


    struct IntObject : public Object
    {
        int i;

        explicit IntObject(int ii);

        ObjectType
        GetType() override;

        std::string
        ToString() override;
    };
    

    struct FloatObject : public Object
    {
        float f;

        explicit FloatObject(float ff);

        ObjectType
        GetType() override;

        std::string
        ToString() override;
    };


    struct BoolObject : public Object
    {
        bool b;

        explicit BoolObject(bool bb);

        ObjectType
        GetType() override;

        std::string
        ToString() override;
    };
    

    struct StringObject : public Object
    {
        std::string s;

        explicit StringObject(const std::string& ss);

        ObjectType
        GetType() override;

        std::string
        ToString() override;
    };
}

#endif  // FEL_OBJECT_H
