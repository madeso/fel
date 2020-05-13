#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>
#include <optional>
#include <functional>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"

#include "lsp/lsp.h"

#include "logger/logger.h"

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
    std::string log_file = "fel-lsp.log";
};


#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#define SET_BINARY_MODE(handle) _setmode(handle, O_BINARY)
#else
#define SET_BINARY_MODE(handle) ((void)0)
#endif


std::string
ToString(const rapidjson::Value& d, bool pretty)
{
    std::ostringstream ss;
    rapidjson::OStreamWrapper osw(ss);

    if(pretty)
    {
        rapidjson::PrettyWriter writer(osw);
        d.Accept(writer);
    }
    else
    {
        rapidjson::Writer writer(osw);
        d.Accept(writer);
    }

    return ss.str();
}


void
Send(const rapidjson::Document& doc)
{
    const auto body = ToString(doc, false);

    std::cout
        << "Content-Length: " << body.length() << "\r\n"
        << "\r\n"
        << body;
}


void
SetId(rapidjson::Value* dst, const rapidjson::Value& v, rapidjson::Document* doc)
{
    rapidjson::Value val;
    if(v.IsString())
    {
        val.SetString(std::string(val.GetString()), doc->GetAllocator());
    }
    else
    {
        val.SetInt(v.GetInt());
    }
    dst->AddMember("id", val, doc->GetAllocator());
}


void
SendNullResponse(const rapidjson::Value& id)
{
    rapidjson::Document doc;
    doc.SetObject();
    SetId(&doc, id, &doc);
    doc["result"] = rapidjson::Value();
    Send(doc);
}


int
RunLanguageServer(const std::string& log_file)
{
    // make std::cin binary: https://stackoverflow.com/a/11259588/180307
    SET_BINARY_MODE(_fileno(stdin));

    auto logger = Logger{log_file};

    logger.WriteInfo("lsp startup");

    bool got_shutdown = false;

    rapidjson::Document message;
    while
    (
        ReadMessageJson
        (
            std::cin,
            &message,
            [&](const std::string& err){logger.WriteError(err);}
        )
    )
    {
        const std::string rpc = message["jsonrpc"].GetString();
        if(rpc != "2.0")
        {
            logger.WriteError("Invalid version");
            continue;
        }
        const std::string method = message["method"].GetString();

        if(method == "initialize")
        {
            logger.WriteInfo("todo: handle init");
        }
        else if(method == "shutdown")
        {
            logger.WriteInfo("shutting down");
            got_shutdown = true;
            SendNullResponse(message["id"]);
        }
        else if(method == "exit")
        {
            logger.WriteInfo("exiting lsp");
            if(got_shutdown)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            logger.WriteInfo("Unknown method: " + method);
        }
    }

    logger.WriteError("end of input");
    return -1;
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
            << app << " -h             print theese instructions.\n"
            << app << " [lsp] --lsp    run as a language server.\n"
            << app << " [options] FILE/CODE/stdin\n"
            << aaa << "                run code\n"
            << "\n"
            << "options:\n"
            << "  -s     make silent\n"
            << "  -S     make super silent\n"
            << "\n"
            << "lsp:\n"
            << "  --log  rotating log for language server to use\n"
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
            if(a == "help" || a == "h")
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
            else if(a == "-log")
            {
                next_option = [&](const std::string& v)
                {
                    opt.log_file = v;
                };
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

    if(next_option.has_value())
    {
        std::cerr << "missing value";
        return -2;
    }
    
    return 0;
}
