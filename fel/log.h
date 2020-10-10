#ifndef FEL_LOG_H
#define FEL_LOG_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "fel/location.h"
#include "fel/where.h"

namespace fel
{
    struct FilePointer;

    namespace log
    {
        enum class Intensity
        {
            Note, Warning, Error
        };

        enum class Type
        {
            EosInString, UnknownCharacter, MissingCloseParen, ExpectedExpression
        };

        struct Entry
        {
            Entry
            (
                const Where& where,
                Intensity intensity,
                log::Type type,
                const std::vector<std::string>& args,
                const std::vector<std::string>& a_debug_context
            );

            Where where;
            Intensity intensity;
            Type type;
            std::vector<std::string> arguments;
            std::vector<std::string> debug_context;
        };

        bool
        operator==(const Entry& lhs, const Entry& rhs);

        std::ostream&
        operator<<(std::ostream& o, const Entry& entry);
    }

    struct Log
    {
        std::vector<log::Entry> entries;
        
        void AddError
        (
            const FilePointer& where,
            log::Type type,
            const std::vector<std::string>& args
        );

        void AddError
        (
            const Where& where,
            log::Type type,
            const std::vector<std::string>& args
        );

        bool
        IsEmpty() const;

        // return true if it is empty
        operator bool() const;
    };

    std::ostream&
    operator<<(std::ostream& o, const Log& log);
}

#endif  // FEL_LOG_H
