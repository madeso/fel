#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>
#include <optional>
#include <functional>
#include <exception>

#include "fel/lexer.h"
#include "fel/file.h"
#include "fel/log.h"

// testing
#include "fel/ast.h"
#include "fel/ast_printer.h"
#include "fel/parser.h"
#include "fel/interpreter.h"

#include "lsp/lsp.h"
#include "lsp/str.h"


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


enum class Mode
{
    Tokenize,
    Parse,

    // not implemented
    Run
};


struct Options
{
    Mode mode = Mode::Run;
    bool treat_file_as_code = false;
    bool print_log = true;
    bool print_output = true;
    std::string log_file = "fel-lsp.log";
};


std::optional<File>
ReadFile(const std::string& path, const Options& opt)
{
    if(opt.treat_file_as_code)
    {
        return File{"commandline", path};
    }
    else if(path == "stdin")
    {
        const auto content = std::string
        (
            std::istreambuf_iterator<char>(std::cin),
            std::istreambuf_iterator<char>()
        );
        return File{"stdin", content};
    }
    else if(auto t = std::ifstream{path.c_str()}; t.good())
    {
        const auto content = std::string
        (
            std::istreambuf_iterator<char>(t),
            std::istreambuf_iterator<char>()
        );
        return File{path, content};
    }
    else
    {
        std::cerr << "Failed to open " << path << "\n";
        return std::nullopt;
    }
}


#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#define SET_BINARY_MODE(handle) _setmode(handle, O_BINARY)
#else
#define SET_BINARY_MODE(handle) ((void)0)
#endif


int
RunLanguageServer(const std::string& log_path)
{
    // make std::cin binary: https://stackoverflow.com/a/11259588/180307
    SET_BINARY_MODE(_fileno(stdin));

    auto log_file = std::ofstream{log_path};
    if(!log_file.good())
    {
        return -2;
    }

    auto logger = [&log_file](const std::string& category, const std::string& to_log)
    {
        log_file << category << ": " << to_log << "\n";
        log_file.flush();
    };

    auto write_error = [&](const std::string& err)
    {
        logger("error", err);
    };
    auto write_info = [&](const std::string& info)
    {
        logger("info", info);
    };

    auto interface = LspInterfaceCallback{write_error, write_info};

    write_info("lsp startup");

    try
    {
        nlohmann::json message;
        while
        (
            ReadMessageJson
            (
                std::cin,
                &message,
                write_error
            )
        )
        {
            auto recieved = interface.Recieve(message);
            if(recieved)
            {
                write_info("exiting as requested");
                return *recieved;
            }
        }
        write_error("end of input");
    }
    catch(const std::exception& ex)
    {
        write_error(Str() << "Exception: " << ex.what());
    }
    catch(...)
    {
        write_error("exiting due to unknown exception");
    }
    
    return -1;
}


int
HandleTokenize(const File& file, const Options& opt)
{
    Log log;
    auto reader = LexerReader{file, &log};
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


int
HandleParse(const File& file, const Options& opt)
{
    Log log;
    auto parser = Parser{file, &log};
    auto parsed_expression = parser.Parse();

    if(opt.print_log)
    {
        Print(log);
    }

    if(opt.print_output && parsed_expression != nullptr)
    {
        std::cout << AstPrinter{}.Visit(parsed_expression.get()) << "\n";
    }

    return log.IsEmpty() && parsed_expression != nullptr ? 0 : -1;
}


int
HandleRun(const File& file, const Options& opt)
{
    Log log;
    auto parser = Parser{file, &log};
    auto parsed_expression = parser.Parse();

    if(opt.print_log) { Print(log); }

    if(!(log.IsEmpty() && parsed_expression != nullptr))
    {
        return -1;
    }

    auto interpreter = Interpreter{&log};
    auto result = interpreter.Evaluate(parsed_expression);
    
    if(opt.print_log) { Print(log); }

    if(opt.print_output && log.IsEmpty())
    {
        std::cout << Stringify(result) << "\n";
    }
    
    return log.IsEmpty() ? 0 : -2;
}


int
main(int argc, char* argv[])
{
    const auto app = std::string(argv[0]);
    const auto PrintUsage = [app]()
    {
        auto aaa = std::string(app.size(), ' ');
        std::cout
            << "-----------------------------------------------\n"
            << "Fast Embedded Lightweight terminal application.\n"
            << "-----------------------------------------------\n"
            << app << " -h               print theese instructions\n"
            << app << " [lsp] --lsp      run as a language server\n"
            << app << " [options] FILE   run code\n"
            << "\n"
            << "FILE can either be\n"
            << "  * a path to a file\n"
            << "  * standard input, if the stdin keyword is passed\n"
            << "  * code to 'execute', if --code is passed\n"
            << "\n"
            << "options:\n"
            << "  -s     make silent\n"
            << "  -S     make super silent\n"
            << "  --code the FILE is not a file but code\n"
            << "\n"
            << "mode selection:\n"
            << "  --tokenize   instead of running, just tokenize the input\n"
            << "  --parse      instead of running, just parse the input\n"
            << "\n"
            << "lsp:\n"
            << "  --log  log for language server to use\n"
            << "\n"
            ;
    };

    if(argc == 1)
    {
        PrintUsage();
        return 0;
    }
    Options opt;
    std::optional<std::function<void (const std::string&)>> next_option = std::nullopt;
    for(int i=1; i<argc; i+=1)
    {
        if(next_option.has_value())
        {
            next_option.value()(argv[i]);
            next_option = std::nullopt;
        }
        else if(IsArgument(argv[i]))
        {
            const auto a = std::string(argv[i]).substr(1);
            if(a == "help" || a == "h" || a == "-help" || a == "?" || a == "-?")
            {
                PrintUsage();
                return 0;
            }
            else if(a == "s")
            {
                opt.print_output = false;
            }
            else if(a == "S")
            {
                opt.print_output = false;
                opt.print_log = false;
            }
            else if(a == "-code")
            {
                opt.treat_file_as_code = true;
            }
            else if(a == "-log")
            {
                next_option = [&](const std::string& v)
                {
                    opt.log_file = v;
                };
            }
            else if(a =="-tokenize")
            {
                opt.mode = Mode::Tokenize;
            }
            else if(a =="-parse")
            {
                opt.mode = Mode::Parse;
            }
            else if(a == "-lsp")
            {
                // todo(Gustav): get log from cmdline
                return RunLanguageServer(opt.log_file);
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
            if(const auto file = ReadFile(argv[i], opt))
            {
                const int return_value = [&]()
                {
                    switch(opt.mode)
                    {
                    case Mode::Tokenize:
                        return HandleTokenize(*file, opt);
                    case Mode::Parse:
                        return HandleParse(*file, opt);
                    case Mode::Run:
                        return HandleRun(*file, opt);
                    default:
                        std::cerr << "Unknown mode!\n";
                        return -2;
                    }
                }();

                if(return_value != 0)
                {
                    return return_value;
                }
            }

            opt = Options{};
        }
    }

    if(next_option.has_value())
    {
        std::cerr << "missing value";
        return -2;
    }
    
    return 0;
}
