#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "fel/lexer.h"
#include "fel/file.h"
#include "fel/log.h"


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


struct Options
{
    bool print_log = true;
    bool print_output = true;
};


#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#define SET_BINARY_MODE(handle) _setmode(handle, O_BINARY)
#else
#define SET_BINARY_MODE(handle) ((void)0)
#endif


// todo(Gustav): move to a lsp library so we can test it

std::istream&
ReadHeaderLine(std::istream& in, std::string* line)
{
    assert(line);

    char c = 0;
    char last = 0;

    std::ostringstream str;

    auto set_line = [&]()
    {
        *line = str.str();
    };

    while(in && in.get(c))
    {
        if(last == '\r' && c == '\n')
        {
            set_line();
            return in;
        }
        if(last != 0)
        {
            str << last;
        }
        last = c;
    }

    set_line();

    SPDLOG_ERROR("eol in headerline");
    return in;
}


using Header = std::map<std::string, std::string>;


std::istream&
ReadHeader(std::istream& in, Header* header)
{
    assert(header);
    std::string line;

    while(in && ReadHeaderLine(in, &line))
    {
        if(line.empty())
        {
            // empty line = end of header
            SPDLOG_INFO("empty line in header -> closing header");
            return in;
        }

        const auto colon = line.find(':');
        if(colon == std::string::npos)
        {
            SPDLOG_ERROR("Missing colon in '{}'", line);
            continue;
        }
        const auto key = line.substr(0, colon);
        const auto value = line.substr(colon + 1);
        SPDLOG_INFO("recieved header '{}' '{}'", key, value);
        (*header)[key] = value;
    }

    SPDLOG_ERROR("eol in header");
    return in;
}


std::string
ReadMessage(std::istream& in, std::size_t length)
{
    std::ostringstream ss;
    std::size_t i = 0;
    
    for(; in && i < length; i += 1)
    {
        char c = 0;
        if(in.get(c))
        {
            ss << c;
        }
    }

    const auto r = ss.str();
    SPDLOG_INFO("Got {} {}", i, r);
    return r;
}

std::istream&
ReadMessage(std::istream& in, std::string* message)
{
    assert(message);
    auto header = Header {};
    while(in && ReadHeader(in, &header))
    {
        // todo(Gustav): check content-type and verify utf8
        const auto found_length = header.find("Content-Length");
        if(found_length == header.end())
        {
            // error
            SPDLOG_ERROR("missing content-length");
            continue;
        }

        auto in_length = std::istringstream(found_length->second);
        std::size_t length = 0;
        in_length >> length;
        SPDLOG_INFO("reading {}", length);
        *message = ReadMessage(in, length);
    }

    return in;
}


void
RunLanguageServer(const std::string& log_file)
{
    // make std::cin binary: https://stackoverflow.com/a/11259588/180307
    SET_BINARY_MODE(_fileno(stdin));

    auto max_size = 1048576 * 5;
    auto max_files = 3;
    auto logger = spdlog::rotating_logger_mt("fel-lsp", log_file, max_size, max_files);
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(3));

    std::string message;
    while(ReadMessage(std::cin, &message))
    {
        // todo(Gustav): parse json and respond...
        SPDLOG_INFO("recieved {}", message);
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
            << "  -s make silent\n"
            << "  -S make super silent\n"
            << "\n"
            ;
    };

    if(argc == 1)
    {
        PrintUsage();
        return 0;
    }
    Options opt;
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
            else if(a == "silent" || a == "s")
            {
                opt.print_output = false;
            }
            else if(a == "supersilent" || a == "S")
            {
                opt.print_output = false;
                opt.print_log = false;
            }
            else if(a == "-lsp")
            {
                // todo(Gustav): get log from cmdline
                RunLanguageServer("C:\\WorkingFolder\\mytemp\\fel.log");
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
                if(opt.print_log)
                {
                    Print(log);
                }
                if(opt.print_output)
                {
                    if(log.IsEmpty())
                    {
                        for(auto token : tokens)
                        {
                            std::cout << token << "\n";
                        }
                    }
                }

                return log.IsEmpty() ? 0 : -1;
            }

            opt = Options{};
        }
    }
    
    return 0;
}
