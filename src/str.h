#ifndef STR_H
#define STR_H

#include <sstream>

namespace fel
{
  struct Str
  {
    std::ostringstream ss;

    template<typename T>
    Str& operator<<(const T& t)
    {
      ss << t;
      return *this;
    }

    operator const std::string() const
    {
      return ss.str();
    }
  };
}

#endif  // STR_H
