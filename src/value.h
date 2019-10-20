#ifndef FEL_VALUE_H
#define FEL_VALUE_H

#include <string>

namespace fel
{
  struct IntValue;
  struct StringValue;

  struct Value
  {
    virtual ~Value();

    virtual IntValue* AsInt();
    virtual StringValue* AsString();

    virtual std::string GetStringRepresentation() const = 0;
    virtual const char* TypeToString() const = 0;
  };

  struct IntValue : public Value
  {
    int value;

    explicit IntValue(int v);
    ~IntValue();

    IntValue* AsInt() override;
    std::string GetStringRepresentation() const override;
    const char* TypeToString() const override;
  };

  struct StringValue : public Value
  {
    std::string value;

    explicit StringValue(const std::string& v);
    ~StringValue();

    StringValue* AsString() override;
    std::string GetStringRepresentation() const override;
    const char* TypeToString() const override;
  };

}

#endif  // FEL_VALUE_H

