#ifndef FEL_OBJECT_H
#define FEL_OBJECT_H

#include <string>
#include <memory>


namespace fel
{
    struct Object
    {
        virtual ~Object() = default;

        virtual std::string ToString() = 0;

        static std::shared_ptr<Object> FromInt(int i);
        static std::shared_ptr<Object> FromFloat(float f);
        static std::shared_ptr<Object> FromBool(bool b);
        static std::shared_ptr<Object> FromString(const std::string& str);
    };
}

#endif  // FEL_OBJECT_H
