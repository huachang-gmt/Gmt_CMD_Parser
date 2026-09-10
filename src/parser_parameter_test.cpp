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
        {"INS 1",                              ParserResult::VALID},
        {"SAH M01 M02 M03",                    ParserResult::VALID},
        {"SHC M01 2 17 400000 40000 0 128",  ParserResult::VALID},
        {"SHC? M01",                           ParserResult::VALID},
        {"VLS 0.15",                           ParserResult::VALID},
        {"MOV R 2000 2000 1000 0 0.5 1",     ParserResult::VALID},
        {"MRV R 2000 2000 1000 0 0.5 1",     ParserResult::VALID},
        {"MSV M02 2000",                       ParserResult::VALID},
        {"MSR M06 1",                          ParserResult::VALID},
        {"MPV M01 M03 1200.0 -35.5",         ParserResult::VALID},
        {"MPR M01 M03 1200.0 -35.5",         ParserResult::VALID},
        {"SPI R U 10 5 2",                     ParserResult::VALID},
        {"DFRS ScanRoutine01",                 ParserResult::VALID},
        {"FLM M10 0.2 V 0.01 TH 255",        ParserResult::VALID},
        {"FLM M08 2 V 0.2",                    ParserResult::VALID},
        {"BKN 0.015",                          ParserResult::VALID}
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
