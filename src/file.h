#ifndef FILE_H
#define FILE_H

#include <string>
#include <optional>

#include "location.h"

namespace fel
{
    struct File
    {
        std::string            filename;
        std::string            data;

        File(const std::string& a_filename, const std::string& a_content);
        static std::optional<File> Open(const std::string& a_filename);
    };

    struct FilePointer
    {
        const File& file;
        std::string::size_type next_index = 0;
        Location               location   = Location {1, 0};

        explicit FilePointer(const File& file);

        bool
        HasMore() const;

        char
        Read();

        // 1 is next character, 2 is the one after that...
        char
        Peek(int advance = 1) const;
    };
}  // namespace fel

#endif  // FILE_H
