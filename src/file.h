#ifndef FILE_H
#define FILE_H

#include <string>

#include "location.h"

namespace fel
{
    struct File
    {
        std::string            filename;
        std::string            data;
        std::string::size_type next_index = 0;
        Location               location   = Location {1, 0};

        File(const std::string& a_filename, const std::string& a_content);

        bool
        HasMore() const;

        char
        Read();

        char
        Peek(int advance = 1) const;
    };
}  // namespace fel

#endif  // FILE_H
