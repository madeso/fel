#ifndef FEL_LOG_H
#define FEL_LOG_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "location.h"

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
            EosInString
        };

        struct Entry
        {
            Entry(const FilePointer& where, Intensity intensity, log::Type type, const std::vector<std::string>& args);

            std::string file;
            Location    location;
            Intensity intensity;
            Type type;
            std::vector<std::string> arguments;
        };

        bool
        operator==(const Entry& lhs, const Entry& rhs);

        std::ostream&
        operator<<(std::ostream& o, const Entry& entry);
    }

    struct Log
    {
        std::vector<log::Entry> entries;

        void AddError(const FilePointer& where, log::Type type, const std::vector<std::string>& args);

        bool IsEmpty() const;
    };

    std::ostream&
    operator<<(std::ostream& o, const Log& log);
}  // namespace fel

#endif  // FEL_LOG_H
