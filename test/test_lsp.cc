#include "catch.hpp"

#include <sstream>

#include "lsp/lsp.h"


using namespace fel;
using Catch::Matchers::Equals;


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

    SECTION("read header line")
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
}
