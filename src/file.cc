#include "file.h"

namespace fel
{
    File::File(const std::string& f, const std::string& d)
        : filename(f), data(d)
    {}

    bool
    File::HasMore() const
    {
        return next_index < data.size();
    }

    char
    File::Read()
    {
        if(next_index < data.size())
        {
            const auto read = data[next_index];
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
    File::Peek(int advance) const
    {
        const auto index = (next_index - 1) + advance;
        if(index < data.size())
            return data[index];
        else
            return 0;
    }
}  // namespace fel
