#include "log.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

#include "file.h"

namespace fel::log
{
    bool
    operator==(const Entry& lhs, const Entry& rhs)
    {
        return lhs.file == rhs.file
            && lhs.location == rhs.location
            && lhs.intensity == rhs.intensity
            && lhs.type == rhs.type
            && lhs.arguments == rhs.arguments;
    }

    Entry::Entry(const FilePointer& a_where, Intensity a_intensity, log::Type a_type, const std::vector<std::string>& a_args)
    : file(a_where.file.filename)
    , location(a_where.location)
    , intensity(a_intensity)
    , type(a_type)
    , arguments(a_args)
    {}
}

namespace fel
{
    std::ostream&
    operator<<(std::ostream& o, const Log& log)
    {
        return o;
    }

    void
    Log::AddError(const FilePointer& where, log::Type type, const std::vector<std::string>& args)
    {
        entries.emplace_back(where, log::Intensity::Error, type, args);
    }

    bool
    Log::IsEmpty() const 
    {
        return entries.empty();
    }
}  // namespace fel
