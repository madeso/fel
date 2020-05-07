#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "lexer.h"
#include "file.h"
#include "log.h"


using namespace fel;


void
Print(const Log& log)
{
    if(!log.IsEmpty())
    {
        std::cerr << log;
    }
}


bool IsArgument(char* s)
{
    switch(s[0])
    {
    case '-':
    case '/':
        return true;
    default:
        return false;
    }
}


std::optional<File>
ReadFile(const std::string& path)
{
    if(path == "stdin")
    {
        const std::string content(
                (std::istreambuf_iterator<char>(std::cin)),
                std::istreambuf_iterator<char>());
        return File {"stdin", content};
    }
    else if(auto t = std::ifstream{path.c_str()}; t.good())
    {
        const std::string content(
                (std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());
        return File {path, content};
    }
    else
    {
        std::cerr << "Failed to open " << path << "\n";
        return std::nullopt;
    }
}


int
main(int argc, char* argv[])
{
    const auto app = std::string(argv[0]);
    const auto PrintUsage = [app]()
    {
        auto aaa = std::string(app.size(), ' ');
        std::cout
            << "Fast Embedded Lightweight terminal application.\n"
            << "----------------------------------------------\n"
            << "\n"
            << app << " -h        print theese instructions.\n"
            << app << " [options] FILE/CODE/stdin\n"
            << aaa << "               run code\n"
            << "\n"
            << "options:\n"
            << "  (none)\n"
            << "\n"
            ;
    };

    if(argc == 1)
    {
        PrintUsage();
        return 0;
    }
    for(int i=1; i<argc; i+=1)
    {
        if(IsArgument(argv[i]))
        {
            const auto a = std::string(argv[i]).substr(1);
            if(a == "help" || a == "h")
            {
                PrintUsage();
                return 0;
            }
            else
            {
                std::cerr << "Invalid option: " << argv[i] << "\n";
                PrintUsage();
                return -1;
            }
        }
        else
        {
            if(const auto file = ReadFile(argv[i]))
            {
                Log log;
                auto reader = LexerReader{*file, &log};
                auto tokens = GetAllTokensInFile(&reader);
                Print(log);
                if(log.IsEmpty())
                {
                    for(auto token : tokens)
                    {
                        std::cout << token << "\n";
                    }
                }
            }
        }
    }
    
    return 0;
}
