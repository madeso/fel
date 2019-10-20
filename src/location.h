#ifndef LOCATION_H
#define LOCATION_H

namespace fel
{
    struct Location
    {
        int line;
        int column;

        Location(int l = -1, int c = -1);
        bool
        operator==(const Location& rhs) const;
    };
}  // namespace fel


#endif  // LOCATION_H
