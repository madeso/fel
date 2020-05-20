#ifndef FEL_STR_H
#define FEL_STR_H

#include <string>
#include <sstream>

namespace fel
{
    struct Str
    {
        std::ostringstream ss;

        operator std::string() const;

        template<typename T>
        Str&
        operator<<(const T& t)
        {
            ss << t;
            return *this;
        }
    };
}

#endif  // FEL_STR_H
