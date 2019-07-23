#include <iostream>
#include <sstream>
#include <string>

#include "fel.h"

using namespace fel;

void Print(const Log& log)
{
  if(false == IsEmpty(log))
  {
    std::cerr << log;
  }
}

void PrintUsage(const std::string& app)
{
  std::cout << "Usage: \n"\
    << "  run file:      " << app << " FILE\n"
    << "  run code:      " << app << " -x CODE\n"
    << "  interactive:   " << app << " -i\n"
    ;
}

int main(int argc, char* argv[])
{
  if(argc < 2 ) {
    std::cerr << "No file specified\n";
    PrintUsage(argv[0]);
    return -2;
  }
  const std::string cmdlineFile = "cmdline";
  Fel fel;
  fel.SetFunction("print",
      [](State* s)
      {
        for(int i=0; i<s->arguments; i+=1)
        {
          std::cout << s->GetArg(i)->GetStringRepresentation();
        }
        std::cout << "\n";
        return 0;
      }
    );
  Log log;
  if(argv[1][0]=='-')
  {
    switch(argv[1][1])
    {
      case 'i':
        {
          std::cout << "Ctrl+Z/Ctrl+D to run> ";
          std::ostringstream ss;
          std::string line;
          ss << std::cin.rdbuf();
          fel.LoadAndRunString(ss.str(), cmdlineFile, &log);
          Print(log);
        }
        return 0;
      case 'x':
        if(argc<3)
        {
          std::cerr << "-x missing expression\n";
          return -3;
        }
        fel.LoadAndRunString(argv[2], cmdlineFile, &log);
        Print(log);
        return 0;
      default:
        std::cerr << "Command not recongnized " << argv[1] << "\n";
        PrintUsage(argv[0]);
        return -4;
    }
  }
  else
  {
    fel.LoadAndRunFile(argv[1], &log);
    Print(log);
  }
  return 0;
}

