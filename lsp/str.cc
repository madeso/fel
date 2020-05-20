#include "lsp/str.h"

namespace fel
{
    Str::operator std::string() const
    {
        return ss.str();
    }
}

