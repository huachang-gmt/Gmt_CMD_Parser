#include "command_parser.h"

#include <iostream>
#include <string>
#include <vector>

struct TestCase
{
    std::string input;
    ParserResult expected;
};

int main()
{
    const std::vector<TestCase> tests =
    {
        {"STP",  ParserResult::VALID},
        {"SVO",  ParserResult::VALID},
        {"SVF",  ParserResult::VALID},
        {"CAL",  ParserResult::VALID},
        {"DSC",  ParserResult::VALID},

        {"MOV?", ParserResult::VALID},
        {"POS?", ParserResult::VALID},
        {"PMS?", ParserResult::VALID},
        {"SPI?", ParserResult::VALID},
        {"FRS?", ParserResult::VALID},
        {"BKN?", ParserResult::VALID},

        {"STP 123", ParserResult::INVALID},
        {"SVO ABC", ParserResult::INVALID},
        {"MOV? 123", ParserResult::INVALID},
        {"UNKNOWN", ParserResult::INVALID},
        {"", ParserResult::INVALID}
    };

    CommandParser parser;

    int passCount = 0;

    for (const TestCase& test : tests)
    {
        const ParseResult result = parser.parse(test.input);

        const bool pass = (result.result == test.expected);

        std::cout
            << (pass ? "[PASS] " : "[FAIL] ")
            << "\""
            << test.input
            << "\""
            << " -> "
            << (result.result == ParserResult::VALID
                    ? "VALID"
                    : "INVALID");

        if (!pass)
        {
            std::cout << "  ERROR: " << result.error;
        }

        std::cout << std::endl;

        if (pass)
        {
            ++passCount;
        }
    }

    std::cout << std::endl;
    std::cout << "[RESULT] "
              << passCount
              << "/"
              << tests.size()
              << " tests passed."
              << std::endl;

    return (passCount == static_cast<int>(tests.size())) ? 0 : 1;
}