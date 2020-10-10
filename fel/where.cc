#include "fel/where.h"

#include <sstream>

#include "fel/file.h"

namespace fel
{
    Where::Where(const std::string& f, const Location& l)
        : file(f)
        , location(l)
    {
    }


    Where::Where(const FilePointer& f)
        : file(f.file.filename)
        , location(f.location)
    {
    }
    

    bool
    operator==(const Where& lhs, const Where& rhs)
    {
        return lhs.file == rhs.file
            && lhs.location == rhs.location
            ;
    }


    std::ostream&
    operator<<(std::ostream& o, const Where& w)
    {
        o  << w.file
            << "("
            << w.location.line << ":" << w.location.column
            << ")";
        
        return o;
    }
}
