#include "lsp/lsp.h"

#include <cassert>
#include <sstream>

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

        set_line();

        error(Str() << "eol in headerline: " << *line);
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
            const auto key = line.substr(0, colon);
            const auto value = line.substr(colon + 1);
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
}

