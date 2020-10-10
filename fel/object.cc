#include "fel/object.h"

#include <sstream>


namespace fel
{

    struct IntObject : public Object
    {
        int i;

        explicit IntObject(int ii) : i(ii) {}

        std::string
        ToString() override
        {
            std::ostringstream ss;
            ss << i;
            return ss.str();
        }
    };
    

    struct FloatObject : public Object
    {
        float f;

        explicit FloatObject(float ff) : f(ff) {}


        std::string
        ToString() override
        {
            std::ostringstream ss;
            ss << f;
            return ss.str();
        }
    };


    struct BoolObject : public Object
    {
        bool b;

        explicit BoolObject(bool bb) : b(bb) {}


        std::string
        ToString() override
        {
            if(b) { return "true";  }
            else  { return "false"; }
        }
    };
    

    struct StringObject : public Object
    {
        std::string s;

        explicit StringObject(const std::string& ss) : s(ss) {}

        std::string
        ToString() override
        {
            return s;
        }
    };


    std::shared_ptr<Object> Object::FromInt(int i)
    {
        return std::make_shared<IntObject>(i);
    }


    std::shared_ptr<Object> Object::FromFloat(float f)
    {
        return std::make_shared<FloatObject>(f);
    }


    std::shared_ptr<Object> Object::FromBool(bool b)
    {
        return std::make_shared<BoolObject>(b);
    }


    std::shared_ptr<Object> Object::FromString(const std::string& str)
    {
        return std::make_shared<StringObject>(str);
    }
}
