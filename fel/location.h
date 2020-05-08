#ifndef FEL_LOCATION_H
#define FEL_LOCATION_H

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


#endif  // FEL_LOCATION_H
