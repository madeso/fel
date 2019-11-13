#include "context.h"

namespace fel
{
    Scope::Scope(Context* a_context, const std::string& t) : context(a_context)
    {
        context->stack.push_back(t);
    }

    Scope::~Scope()
    {
        context->stack.pop_back();
    }
}
