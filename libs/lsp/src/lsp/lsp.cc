#include "lsp/lsp.h"

#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "fmt/core.h"


namespace fel
{
    std::istream&
    ReadHeaderLine(std::istream& in, std::string* line, ErrorFunction error)
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

        str << last;
        set_line();

        error(fmt::format("eof in headerline: {}", *line));
        return in;
    }


    std::istream&
    ReadHeader(std::istream& in, Header* header, ErrorFunction error)
    {
        assert(header);
        std::string line;

        while(in && ReadHeaderLine(in, &line, error))
        {
            if(line.empty())
            {
                // empty line = end of header
                return in;
            }

            const auto colon = line.find(':');
            if(colon == std::string::npos)
            {
                error(fmt::format("Missing colon in '{}'", line));
                continue;
            }
            if(colon+2 >= line.length())
            {
                error(fmt::format("No value in '{}'", line));
                continue;
            }
            if(line[colon+1] != ' ')
            {
                error(fmt::format("Missing space in '{}'", line));
                continue;
            }
            const auto key = line.substr(0, colon);
            const auto value = line.substr(colon + 2);
            (*header)[key] = value;
        }

        error("eol in header");
        return in;
    }


    std::string
    ReadMessageBody(std::istream& in, std::size_t length)
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
            else
            {
                break;
            }
        }

        const auto r = ss.str();
        return r;
    }


    std::istream&
    ReadMessage(std::istream& in, std::string* message, ErrorFunction error)
    {
        assert(message);
        auto header = Header {};
        if(in && ReadHeader(in, &header, error))
        {
            // todo(Gustav): check content-type and verify utf8
            const auto found_length = header.find("Content-Length");
            if(found_length == header.end())
            {
                // error
                error("missing content-length");
                return in;
            }

            auto in_length = std::istringstream(found_length->second);
            std::size_t length = 0;
            in_length >> length;
            *message = ReadMessageBody(in, length);
        }

        return in;
    }


    std::istream&
    ReadMessageJson(std::istream& in, nlohmann::json* message, ErrorFunction error)
    {
        assert(message);
        std::string source;
        ReadMessage(in, &source, error);
        if(!in)
        {
            return in;
        }

        try
        {
            *message = nlohmann::json::parse(source);
        }
        catch(nlohmann::json::parse_error& e)
        {
            // output exception information
            error
            (
                fmt::format("json parse error: {} id: {} byte position of error: {}", e.what(), e.id, e.byte)
            );
        }
        return in;
    }


    std::string
    ToString(const nlohmann::json& d, bool pretty)
    {
        if(pretty)
        {
            std::ostringstream ss;
            ss << std::setw(4);
            ss << d;
            return ss.str();
        }
        else
        {
            return d.dump();
        }
    }


    void
    LspInterface::SendNullResponse(const nlohmann::json& id)
    {
        nlohmann::json doc;
        doc["id"] = id;
        doc["result"] = nlohmann::json();
        Send(doc);
    }


    std::optional<std::string>
    GetOptionalString(const nlohmann::json& m, const std::string& key)
    {
        const auto f = m.find(key);
        if(f == m.end())
        {
            return std::nullopt;
        }

        return f->get<std::string>();
    }


    std::optional<int>
    LspInterface::Recieve(const nlohmann::json& message)
    {
        const auto rpc = GetOptionalString(message, "jsonrpc").value_or("missing rpc version");
        if(rpc != "2.0")
        {
            error("Invalid version");
            return std::nullopt;
        }
        const auto method = GetOptionalString(message, "method").value_or("missing method");

        if(method == "initialize")
        {
            info("todo: handle init");
        }
        else if(method == "shutdown")
        {
            info("shutting down");
            got_shutdown = true;
            SendNullResponse(message["id"]);
        }
        else if(method == "exit")
        {
            info("exiting lsp");
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
            error("Unknown method: " + method);
        }

        return std::nullopt;
    }


    LspInterfaceCallback::LspInterfaceCallback(ErrorFunction e, ErrorFunction i)
        : error_callback(e)
        , info_callback(i)
    {
    }


    void
    LspInterfaceCallback::error(const std::string& err)
    {
        error_callback(err);
    }


    void
    LspInterfaceCallback::info(const std::string& info)
    {
        info_callback(info);
    }


    void
    LspInterfaceCallback::Send(const nlohmann::json& doc)
    {
        const auto body = ToString(doc, false);

        std::cout
            << "Content-Length: " << body.length() << "\r\n"
            << "\r\n"
            << body;
    }
}

