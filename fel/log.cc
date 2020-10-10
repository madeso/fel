#include "log.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

#include "fel/file.h"

namespace fel::log
{
    bool
    operator==(const Entry& lhs, const Entry& rhs)
    {
        return lhs.where == rhs.where
            && lhs.intensity == rhs.intensity
            && lhs.type == rhs.type
            && lhs.arguments == rhs.arguments;
    }


    Entry::Entry
    (
        const Where& a_where,
        Intensity a_intensity,
        log::Type a_type,
        const std::vector<std::string>& a_args,
        const std::vector<std::string>& a_debug_context
    )
        : where(a_where)
        , intensity(a_intensity)
        , type(a_type)
        , arguments(a_args)
        , debug_context(a_debug_context)
    {
    }


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
        std::string
        Arg(const Entry& entry, size_t i)
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
        o
            << entry.where << " "
            << entry.intensity << ": "
            ;
        switch(entry.type)
        {
        case Type::EosInString:
            assert(entry.arguments.size() == 0);
            o << "'End Of Stream' detected in string";
            break;
        case Type::UnknownCharacter:
            assert(entry.arguments.size() == 1);
            o << "Found unknown character '" << Arg(entry, 0) << "'";
            break;
        case Type::MissingCloseParen:
            assert(entry.arguments.size() == 0);
            o << "Missing close paren";
            break;
        case Type::ExpectedExpression:
            assert(entry.arguments.size() == 0);
            o << "Expected expression";
            break;
        default:
            o << "Internal error: Unhandled error type in switch.";
            break;
        }
        if(!entry.debug_context.empty())
        {
            o << "\n";
            bool first = true;
            for(auto d: entry.debug_context)
            {
                if(first) first = false;
                else o << "->";
                o << d;
            }
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
    Log::AddError
    (
        const FilePointer& where,
        log::Type type,
        const std::vector<std::string>& args
    )
    {
        AddError(Where{where.file.filename, where.location}, type, args);
    }


    void
    Log::AddError
    (
        const Where& where,
        log::Type type,
        const std::vector<std::string>& args
    )
    {
        std::vector<std::string> dc;
        entries.emplace_back(where, log::Intensity::Error, type, args, dc);
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
}
