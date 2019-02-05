#include <iostream>

#include "fel.h"

using namespace fel;

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
  return 0;
}

