#include <iostream>

#include "fel.h"

using namespace fel;

void Print(const Log& log)
{
  std::cerr << log;
  std::cout << log.entries.size() << " fel error(s) detected\n";
}

int main(int argc, char* argv[])
{
  if(argc != 2 ) {
    std::cout << "No file specified\n";
    std::cerr << "Usage: " << argv[0] << " FILE\n";
    return -2;
  }
  Fel fel;
  Log log;
  fel.LoadAndRunFile(argv[1], &log);
  Print(log);
  return 0;
}
