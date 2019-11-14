#ifndef FEL_CONTEXT_H
#define FEL_CONTEXT_H

#include <string>
#include <vector>

namespace fel
{
    struct Context
    {
        std::vector<std::string> stack;
    };

    struct Scope
    {
        Scope(Context* a_context, const std::string& t);
        ~Scope();

        Context* context;
    };
}

#endif  // FEL_CONTEXT_H
