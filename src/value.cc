#include "value.h"

#include <sstream>

namespace fel
{
  // struct Value
  Value::~Value()
  {
  }

  IntValue* Value::AsInt()
  {
    return nullptr;
  }

  StringValue* Value::AsString()
  {
    return nullptr;
  }


  // struct IntValue : public Value
  IntValue::IntValue(int v) : value (v) {}
  IntValue::~IntValue()
  {
  }

  IntValue* IntValue::AsInt()
  {
    return this;
  }

  std::string IntValue::GetStringRepresentation() const
  {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }


  // struct StringValue : public Value
  StringValue::StringValue(const std::string& v) : value(v) {}
  StringValue::~StringValue()
  {
  }

  StringValue* StringValue::AsString()
  {
    return this;
  }

  std::string StringValue::GetStringRepresentation() const
  {
    return value;
  }
}

