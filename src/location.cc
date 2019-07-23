#include "location.h"

namespace fel
{
  Location::Location(int l, int c) : line(l), column(c) {}

  bool Location::operator==(const Location& rhs) const
  {
    return line == rhs.line && column == rhs.column;
  }
}

