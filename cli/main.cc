#include <iostream>
#include <sstream>
#include <string>

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

enum class FileMode
{
    Normal, Lexer
};

struct Options
{
    FileMode file_mode = FileMode::Normal;
    bool file_as_commandline =false;
};

std::optional<File> ReadFile(const Options& options, const std::string& path)
{
    if(options.file_as_commandline)
    {
        return File{"commandline", path};
    }
    else
    if(path == "stdin")
    {
        const std::string content(
                (std::istreambuf_iterator<char>(std::cin)),
                std::istreambuf_iterator<char>());
        return File {"stdin", content};
    }
    else if(auto f = File::Open(path))
    {
        return *f;
    }
    else
    {
        std::cerr << "Failed to open " << path << "\n";
        return std::nullopt;
    }
}

void RunLexer(const File& file)
{
    Log log;
    auto tokens = GetAllTokensInFile(file, &log);
    Print(log);
    if(log.IsEmpty())
    {
        for(auto token : tokens)
        {
            std::cout << token << "\n";
        }
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
            << "  -lex, -x    print the lexer result instead of running the code\n"
            << "  -code, -c   force code as the argument\n"
            << "\n"
            ;
    };

    Options options;
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
            if(a == "lex" || a == "x")
            {
                options.file_mode = FileMode::Lexer;
            }
            else if(a == "code" || a == "c")
            {
                options.file_as_commandline = true;
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
            if(const auto file = ReadFile(options, argv[i]))
            {
                switch(options.file_mode)
                {
                case FileMode::Normal:
                    std::cerr << "It's currently unsupported to run " << file->filename << "\n";
                    return -1;
                case FileMode::Lexer:
                    RunLexer(*file);
                    break;
                }

                options = Options{};
            }
        }
    }
    
    return 0;
}
