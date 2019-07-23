#ifndef FILE_H
#define FILE_H

#include <string>

#include "location.h"

namespace fel
{
  struct File
  {
    std::string filename;
    std::string data;
    int next_index = 0;
    Location location = Location{1,0};
    
    File(const std::string& f, const std::string& d); 

    bool HasMore() const;

    char Read();

    char Peek(int advance=1) const;
  };
}

#endif  // FILE_H

