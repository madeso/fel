#include "file.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include <cassert>

namespace fel
{
    File::File(const std::string& a_filename, const std::string& a_content)
        : filename(a_filename), data(a_content)
    {}

    std::optional<File> File::Open(const std::string& a_filename)
    {
        std::ifstream t(a_filename.c_str());
        if(!t.good())
        {
            return std::nullopt;
        }
        const std::string content(
                (std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());
        return File{a_filename, content};
    }

    FilePointer::FilePointer(const File& a_file) : file(a_file)
    {
    }

    bool
    FilePointer::HasMore() const
    {
        return next_index < file.data.size();
    }

    char
    FilePointer::Read()
    {
        if(next_index < file.data.size())
        {
            const auto read = file.data[next_index];
            next_index += 1;
            if(read == '\n')
            {
                location.line += 1;
                location.column = 0;
            }
            else
            {
                location.column += 1;
            }
            return read;
        }
        else
        {
            return 0;
        }
    }

    char
    FilePointer::Peek(int advance) const
    {
        assert(advance > 0);
        // assert(next_index > 0);
        const auto index = next_index + advance - 1;
        if(index < file.data.size())
            return file.data[index];
        else
            return 0;
    }
}  // namespace fel
