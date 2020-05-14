#include "lsp/lsp.h"

#include <cassert>
#include <sstream>
#include <iostream>

#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/error/en.h"


namespace fel
{
    struct Str
    {
        std::ostringstream ss;

        operator std::string() const
        {
            return ss.str();
        }

        template<typename T>
        Str&
        operator<<(const T& t)
        {
            ss << t;
            return *this;
        }
    };


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

        error(Str() << "eof in headerline: " << *line);
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
                error(Str() << "Missing colon in '" << line << "'");
                continue;
            }
            if(colon+2 >= line.length())
            {
                error(Str() << "No value in '" << line << "'");
                continue;
            }
            if(line[colon+1] != ' ')
            {
                error(Str() << "Missing space in '" << line << "'");
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
    ReadMessageJson(std::istream& in, rapidjson::Document* message, ErrorFunction error)
    {
        assert(message);
        std::string source;
        ReadMessage(in, &source, error);
        if(!in)
        {
            return in;
        }

        if(message->Parse(source.c_str()).HasParseError())
        {
            const auto offset = static_cast<unsigned>(message->GetErrorOffset());
            const std::string err = rapidjson::GetParseError_En(message->GetParseError());
            error(Str() << "json error(" << offset << "): " << err);
        }

        return in;
    }


    LspInterface::LspInterface(ErrorFunction e, ErrorFunction i)
        : error(e)
        , info(i)
    {
    }

    
    
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
    LspInterface::Send(const rapidjson::Document& doc)
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
    LspInterface::SendNullResponse(const rapidjson::Value& id)
    {
        rapidjson::Document doc;
        doc.SetObject();
        SetId(&doc, id, &doc);
        doc["result"] = rapidjson::Value();
        Send(doc);
    }


    std::optional<int>
    LspInterface::Recieve(const rapidjson::Document& message)
    {
        const std::string rpc = message["jsonrpc"].GetString();
        if(rpc != "2.0")
        {
            error("Invalid version");
            return std::nullopt;
        }
        const std::string method = message["method"].GetString();

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

}

