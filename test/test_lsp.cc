#include "catch.hpp"

#include "falsestring.h"
#include <sstream>
#include <set>

#include "lsp/lsp.h"


using namespace fel;
using Catch::Matchers::Equals;


struct Str
{
    std::ostringstream ss;

    template<typename T>
    Str& operator<<(const T& t)
    {
        ss << t;
        return *this;
    }

    operator std::string() const
    {
        return ss.str();
    }
};


template<typename K, typename V>
FalseString
MapEquals(const std::map<K, V>& lhs, const std::map<K, V>& rhs)
{
    std::set<K> keys_in_rhs;
    for(const auto& s: rhs) { keys_in_rhs.emplace(s.first); }

    std::vector<std::string> errors;

    for(const auto& s: lhs)
    {
        const auto found = rhs.find(s.first);
        if(found == rhs.end())
        {
            errors.push_back(Str() << "missing key " << s.first);
            continue;
        }

        keys_in_rhs.erase(s.first);

        if(s.second != found->second)
        {
            errors.push_back
            (
                Str() << "value different for " << s.first << ": "
                "<" << s.second << ">"
                " != "
                "<" << found->second << ">"
            );
        }
    }

    for(const auto& k: keys_in_rhs)
    {
        errors.push_back(Str() << k << " missing from lhs");
    }

    if(errors.empty()) { return FalseString::True(); }

    std::ostringstream ss;
    bool first = true;
    for(const auto& e: errors)
    {
        if(first) { first = false; }
        else { ss << ", "; }
        ss << e;
    }

    return FalseString::False(ss.str());
}


TEST_CASE("lsp", "[lsp]")
{
    auto errors = std::vector<std::string>{};
    const auto no_errors = std::vector<std::string>{};
    auto add_error = [&](const std::string& e)
    {
        errors.emplace_back(e);
    };
    auto parse = []
    (
        const std::string& src,
        std::function<void (std::istream&)> callback
    )
    -> bool
    {
        std::istringstream stream(src);
        callback(stream);
        return stream.good();
    };

    // todo(Gustav): add more tests!

    SECTION("read header line")
    {
        std::string line;
        const auto parse_result = parse
        (
            "dog is good\r\n",
            [&](std::istream& in)
            {
                ReadHeaderLine(in, &line, add_error);
            }
        );
        CHECK(parse_result);
        CHECK(line == "dog is good");
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                no_errors
            )
        );
    }

    SECTION("read header line with newlines")
    {
        std::string line;
        const auto parse_result = parse
        (
            "dog is good\r\r\n",
            [&](std::istream& in)
            {
                ReadHeaderLine(in, &line, add_error);
            }
        );
        CHECK(parse_result);
        CHECK(line == "dog is good\r");
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                no_errors
            )
        );
    }

    SECTION("read header line missing term")
    {
        std::string line;
        const auto parse_result = parse
        (
            "dog is bad",
            [&](std::istream& in)
            {
                ReadHeaderLine(in, &line, add_error);
            }
        );
        CHECK_FALSE(parse_result);
        CHECK(line == "dog is bad");
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                {
                    "eof in headerline: dog is bad"
                }
            )
        );
    }

    //////////////////////////////////////////////////////////////////////////
    // header

    using PSS = std::pair<std::string, std::string>;

    SECTION("read header single")
    {
        auto header = Header{};
        const auto parse_result = parse
        (
            "dog: good\r\n\r\n",
            [&](std::istream& in)
            {
                ReadHeader(in, &header, add_error);
            }
        );
        CHECK(parse_result);
        CHECK
        (
            MapEquals
            (
                header,
                {
                    PSS("dog", "good")
                }
            )
        );
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                no_errors
            )
        );
    }

    SECTION("read header with extra space")
    {
        auto header = Header{};
        const auto parse_result = parse
        (
            "cat:  awesome\r\n\r\n",
            [&](std::istream& in)
            {
                ReadHeader(in, &header, add_error);
            }
        );
        CHECK(parse_result);
        CHECK
        (
            MapEquals
            (
                header,
                {
                    PSS("cat", " awesome")
                }
            )
        );
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                no_errors
            )
        );
    }

    SECTION("read header no space")
    {
        auto header = Header{};
        const auto parse_result = parse
        (
            "cat:good\r\n\r\n",
            [&](std::istream& in)
            {
                ReadHeader(in, &header, add_error);
            }
        );
        CHECK(parse_result);
        CHECK
        (
            MapEquals
            (
                header,
                {
                }
            )
        );
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                {
                    "Missing space in 'cat:good'"
                }
            )
        );
    }

    SECTION("read header no value")
    {
        auto header = Header{};
        const auto parse_result = parse
        (
            "cat:\r\n\r\n",
            [&](std::istream& in)
            {
                ReadHeader(in, &header, add_error);
            }
        );
        CHECK(parse_result);
        CHECK
        (
            MapEquals
            (
                header,
                {
                }
            )
        );
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                {
                    "No value in 'cat:'"
                }
            )
        );
    }

    SECTION("read header no colon")
    {
        auto header = Header{};
        const auto parse_result = parse
        (
            "catgood\r\n\r\n",
            [&](std::istream& in)
            {
                ReadHeader(in, &header, add_error);
            }
        );
        CHECK(parse_result);
        CHECK
        (
            MapEquals
            (
                header,
                {
                }
            )
        );
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                {
                    "Missing colon in 'catgood'"
                }
            )
        );
    }

    //////////////////////////////////////////////////////////////////////////
    // body

    SECTION("5+2 chars body")
    {
        std::string first_body = "";
        std::string second_body = "";

        const auto parse_result = parse
        (
            "1234567890",
            [&](std::istream& in)
            {
                first_body = ReadMessageBody(in, 5);
                second_body = ReadMessageBody(in, 2);
            }
        );
        CHECK(parse_result);
        CHECK(first_body == "12345");
        CHECK(second_body == "67");
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                no_errors
            )
        );
    }

    SECTION("5 chars body with input that only have 3")
    {
        std::string body = "";

        const auto parse_result = parse
        (
            "123",
            [&](std::istream& in)
            {
                body = ReadMessageBody(in, 5);
            }
        );
        CHECK_FALSE(parse_result);
        CHECK(body == "123");
        CHECK_THAT
        (
            errors,
            Equals<std::string>
            (
                no_errors
            )
        );
    }
}
