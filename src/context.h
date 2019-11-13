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

    #define FEL_CONCAT_HELPER(X,Y) X ## Y
    #define FEL_CONCAT(X, Y) FEL_CONCAT_HELPER(X, Y)
    #define FEL_SCOPE(context, message) auto FEL_CONCAT(scope_, __LINE__) = Scope{context, message}
}

#endif  // FEL_CONTEXT_H
