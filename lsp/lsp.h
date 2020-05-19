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
        bool got_shutdown = false;

        virtual void
        error(const std::string& err) = 0;

        virtual void
        info(const std::string& info) = 0;

        void
        SendNullResponse(const nlohmann::json& id);

        virtual ~LspInterface() = default;

        virtual
        void
        Send(const nlohmann::json& doc) = 0;

        std::optional<int>
        Recieve(const nlohmann::json& doc);
    };

    struct LspInterfaceCallback : public LspInterface
    {
        ErrorFunction error_callback;
        ErrorFunction info_callback;
        bool got_shutdown = false;

        LspInterfaceCallback(ErrorFunction e, ErrorFunction i);

        void
        error(const std::string& err) override;

        void
        info(const std::string& info) override;

        void
        Send(const nlohmann::json& doc);
    };
}

#endif  // FEL_LSP_H
