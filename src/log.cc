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


    std::ostream&
    operator<<(std::ostream& o, const Intensity& i)
    {
        switch(i)
        {
        case Intensity::Note:
            o << "Note";
            return o;
        case Intensity::Warning:
            o << "Warning";
            return o;
        case Intensity::Error:
            o << "Error";
            return o;
        default:
            o << "<internal error: unhandled intensity>";
            return o;
        }
    }

    namespace
    {
        std::string Arg(const Entry& entry, size_t i)
        {
            if(i >= entry.arguments.size())
            {
                assert(false);
                return "<invalid index>";
            }

            return entry.arguments[i];
        }
    }

    std::ostream&
    operator<<(std::ostream& o, const Entry& entry)
    {
        o << entry.file << "(" << entry.location.line << ":" << entry.location.line << ") " << entry.intensity << ": ";
        switch(entry.type)
        {
        case Type::EosInString:
            o << "'End Of Stream' detected in string";
            break;
        case Type::UnexpectedSymbol:
            o << "Unexpected symbol " << Arg(entry, 1) << ", expected " << Arg(entry, 0);
            break;
        case Type::InvalidSymbol:
            o << "Invalid symbol " << Arg(entry, 0);
            break;
        default:
            o << "Internal error: Unhandled error in switch.";
            break;
        }
        return o;
    }
}

namespace fel
{
    std::ostream&
    operator<<(std::ostream& o, const Log& log)
    {
        int errors = 0;
        int warnings = 0;
        for(const auto& e: log.entries)
        {
            o << e << "\n";
            switch(e.intensity)
            {
            case log::Intensity::Note:
                break;
            case log::Intensity::Error:
                errors += 1;
                break;
            case log::Intensity::Warning:
                warnings += 1;
                break;
            }
        }

        o << errors << " error(s) and " << warnings << " warning(s) detected.\n";
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

    Log::operator bool() const
    {
        return IsEmpty();
    }
}  // namespace fel
