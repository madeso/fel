#include "fel/object.h"

#include <sstream>


namespace fel
{
    // ------------------------------------------------------------------------

    IntObject::IntObject(int ii)
        : i(ii)
    {
    }


    ObjectType
    IntObject::GetType()
    {
        return ObjectType::Int;
    }


    std::string
    IntObject::ToString()
    {
        std::ostringstream ss;
        ss << i;
        return ss.str();
    }


    // ------------------------------------------------------------------------
    

    FloatObject::FloatObject(float ff)
        : f(ff)
    {
    }


    ObjectType
    FloatObject::GetType()
    {
        return ObjectType::Number;
    }


    std::string
    FloatObject::ToString()
    {
        std::ostringstream ss;
        ss << f;
        return ss.str();
    }


    // ------------------------------------------------------------------------


    BoolObject::BoolObject(bool bb)
        : b(bb)
    {
    }


    ObjectType
    BoolObject::GetType()
    {
        return ObjectType::Bool;
    }


    std::string
    BoolObject::ToString()
    {
        if(b) { return "true";  }
        else  { return "false"; }
    }


    // ------------------------------------------------------------------------
    

    StringObject::StringObject(const std::string& ss)
        : s(ss)
    {
    }


    ObjectType
    StringObject::GetType()
    {
        return ObjectType::String;
    }


    std::string
    StringObject::ToString()
    {
        return s;
    }


    // ------------------------------------------------------------------------


    std::shared_ptr<Object>
    Object::FromInt(int i)
    {
        return std::make_shared<IntObject>(i);
    }


    std::shared_ptr<Object>
    Object::FromFloat(float f)
    {
        return std::make_shared<FloatObject>(f);
    }


    std::shared_ptr<Object>
    Object::FromBool(bool b)
    {
        return std::make_shared<BoolObject>(b);
    }


    std::shared_ptr<Object>
    Object::FromString(const std::string& str)
    {
        return std::make_shared<StringObject>(str);
    }
}
