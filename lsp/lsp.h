#ifndef FEL_LSP_H
#define FEL_LSP_H

#include <istream>
#include <functional>
#include <map>
#include <string>
#include <optional>

#include "nlohmann/json.hpp"


namespace fel
{
    using Header = std::map<std::string, std::string>;
    using ErrorFunction = std::function<void (const std::string& str)>;

    // all functions assuming in stream is binary
    // and doesn't translate \r\n into endl

    // read text into line until \r\n but doesn't include the ending \r\n
    std::istream&
    ReadHeaderLine(std::istream& in, std::string* line, ErrorFunction error);

    // read 'key: value' lines terminated by \r\n and the final \r\n into header
    std::istream&
    ReadHeader(std::istream& in, Header* header, ErrorFunction error);

    // read a message body of length bytes
    std::string
    ReadMessageBody(std::istream& in, std::size_t length);

    // read a header and the corresponding message body
    std::istream&
    ReadMessage(std::istream& in, std::string* message, ErrorFunction error);

    // read a header and the corresponding message body
    std::istream&
    ReadMessageJson(std::istream& in, nlohmann::json* message, ErrorFunction error);


    struct LspInterface
    {
        ErrorFunction error;
        ErrorFunction info;
        bool got_shutdown = false;

        LspInterface(ErrorFunction e, ErrorFunction i);

        void
        SendNullResponse(const nlohmann::json& id);

        virtual ~LspInterface() = default;

        virtual
        void
        Send(const nlohmann::json& doc);

        std::optional<int>
        Recieve(const nlohmann::json& doc);
    };
}

#endif  // FEL_LSP_H
