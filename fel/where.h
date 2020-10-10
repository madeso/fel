#ifndef FEL_WHERE_H
#define FEL_WHERE_H

#include <string>

#include "fel/location.h"
#include <iostream>


namespace fel
{
    struct FilePointer;

    struct Where
    {
        std::string file = "<undefined>";
        Location location = {};

        Where() = default;
        Where(const std::string& file, const Location& location);
        explicit Where(const FilePointer& file);
    };


    bool
    operator==(const Where& lhs, const Where& rhs);


    std::ostream&
    operator<<(std::ostream& o, const Where& w);
}

#endif  // FEL_WHERE_H