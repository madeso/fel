#ifndef FEL_LOG_H
#define FEL_LOG_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "location.h"

namespace fel
{
    struct LogEntry
    {
        std::string file;
        std::string message;
        Location    location;

        bool
        operator==(const LogEntry& rhs) const;
    };
    std::ostream&
    operator<<(std::ostream& o, const LogEntry& entry);

    struct Log
    {
        std::vector<LogEntry> entries;

        operator bool() const;
    };
    std::ostream&
    operator<<(std::ostream& o, const Log& log);

    void
    Add(Log*               log,
        const std::string& file,
        const Location&    location,
        const std::string& message);
    bool
    IsEmpty(const Log& log);
}  // namespace fel

#endif  // FEL_LOG_H
