#ifndef FEL_H
#define FEL_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "log.h"
#include "value.h"

namespace fel
{
    struct State
    {
        int                                 arguments = 0;
        std::vector<std::shared_ptr<Value>> stack;

        std::shared_ptr<Value>
        GetStack(int index) const;
        std::shared_ptr<Value>
        GetArg(int index) const;
    };

    struct Fel
    {
        using FunctionCallback = std::function<int(State*)>;

        void
        SetFunction(const std::string& name, FunctionCallback callback);
        void
        LoadAndRunString(
                const std::string& str,
                const std::string& filename,
                Log*               log);
        void
        LoadAndRunFile(const std::string& file, Log* log);

        std::map<std::string, FunctionCallback> functions;
    };
}  // namespace fel

#endif  // FEL_H
