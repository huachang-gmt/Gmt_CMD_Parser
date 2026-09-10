#include "command_parser.h"

#include <regex>
#include <string>

namespace
{

constexpr const char* REGEX_NO_PARAMETER =
    R"(^\s*$)";

constexpr const char* REGEX_INTEGER =
    R"([+-]?[0-9]+)";

constexpr const char* REGEX_NUMBER =
    R"([+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+))";

constexpr const char* REGEX_INS =
    R"(^\s*(0|1)\s*$)";

constexpr const char* REGEX_SAH =
    R"(^\s*(A|M[0-9]{2})(\s+(A|M[0-9]{2})){2}\s*$)";

constexpr const char* REGEX_SHC =
    R"(^\s*M[0-9]{2}\s+[+-]?[0-9]+\s+[+-]?[0-9]+\s+[+-]?[0-9]+\s+[+-]?[0-9]+(?:\.[0-9]+)?\s+[+-]?[0-9]+(?:\.[0-9]+)?\s+[+-]?[0-9]+\s*$)";

constexpr const char* REGEX_SHC_QUERY =
    R"(^\s*M[0-9]{2}\s*$)";

constexpr const char* REGEX_VLS =
    R"(^\s*[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)\s*$)";

constexpr const char* REGEX_SPI =
    R"(^\s*[RL]\s+[UVW]\s+[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)\s+[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)\s+[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)\s*$)";

constexpr const char* REGEX_ADC_QUERY =
    R"(^\s*[+-]?[0-9]+\s*$)";

constexpr const char* REGEX_SAC =
    R"(^\s*[+-]?[0-9]+\s*$)";

constexpr const char* REGEX_BKN =
    R"(^\s*[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)\s*$)";

constexpr const char* REGEX_WRD =
    R"(^\s*[+-]?[0-9]+\s+[+-]?[0-9]+\s+[+-]?[0-9]+\s+(1|2|4)\s+[+-]?[0-9]+\s*$)";

constexpr const char* REGEX_RRD =
    R"(^\s*[+-]?[0-9]+\s+[+-]?[0-9]+\s+[+-]?[0-9]+\s+(1|2|4)\s*$)";

constexpr const char* REGEX_MOV =
    R"(^\s*R\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+(\.[0-9]+)?\s+[0-9]+(\.[0-9]+)?\s*$)";

constexpr const char* REGEX_MRV =
    R"(^\s*R\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+(\.[0-9]+)?\s+[0-9]+(\.[0-9]+)?\s*$)";

constexpr const char* REGEX_MSV =
    R"(^\s*M[0-9]{2}\s+[0-9]+\s*$)";

constexpr const char* REGEX_MSR =
    R"(^\s*M[0-9]{2}\s+[0-9]+\s*$)";

constexpr const char* REGEX_MPV =
    R"(^\s*M[0-9]{2}\s+M[0-9]{2}\s+[+-]?[0-9]+(\.[0-9]+)?\s+[+-]?[0-9]+(\.[0-9]+)?\s*$)";

constexpr const char* REGEX_MPR =
    R"(^\s*M[0-9]{2}\s+M[0-9]{2}\s+[+-]?[0-9]+(\.[0-9]+)?\s+[+-]?[0-9]+(\.[0-9]+)?\s*$)";

constexpr const char* REGEX_DFRS =
    R"(^\s*[A-Za-z0-9_]+\s*$)";

constexpr const char* REGEX_FLM =
    R"(^\s*M[0-9]{2}\s+[0-9]+(\.[0-9]+)?\s+V\s+[0-9]+(\.[0-9]+)?(\s+TH\s+[0-9]+)?\s*$)";



struct CommandRule
{
    const char* command;
    const char* parameterRegex;
};

static const CommandRule COMMAND_RULES[] =
{
    {"INS",   REGEX_INS},
    {"STP",   REGEX_NO_PARAMETER},
    {"SAH",   REGEX_SAH},
    {"SHC",   REGEX_SHC},
    {"SHC?",  REGEX_SHC_QUERY},
    {"SVO",   REGEX_NO_PARAMETER},
    {"SVF",   REGEX_NO_PARAMETER},
    {"CAL",   REGEX_NO_PARAMETER},
    {"ERR?",  REGEX_NO_PARAMETER},
    {"DSC",   REGEX_NO_PARAMETER},
    {"VLS",   REGEX_VLS},
    {"VLS?",  REGEX_NO_PARAMETER},
    {"MOV",   REGEX_MOV},
    {"MRV",   REGEX_MRV},
    {"MSV",   REGEX_MSV},
    {"MSR",   REGEX_MSR},
    {"MPV",   REGEX_MPV},
    {"MPR",   REGEX_MPR},
    {"MOV?",  REGEX_NO_PARAMETER},
    {"POS?",  REGEX_NO_PARAMETER},
    {"PMS?",  REGEX_NO_PARAMETER},
    {"SPI",   REGEX_SPI},
    {"SPI?",  REGEX_NO_PARAMETER},
    {"FRS",   nullptr},
    {"FRS?",  REGEX_NO_PARAMETER},
    {"DFRS",  REGEX_DFRS},
    {"FDR",   nullptr},
    {"FSM",   nullptr},
    {"FLM",   REGEX_FLM},
    {"FDG",   nullptr},
    {"ADC?",  nullptr},
    {"SAC",   REGEX_SAC},
    {"BKN",   REGEX_BKN},
    {"BKN?",  REGEX_NO_PARAMETER},
    {"WRD",   REGEX_WRD},
    {"RRD",   REGEX_RRD}
};

constexpr std::size_t COMMAND_RULE_COUNT =
    sizeof(COMMAND_RULES) / sizeof(COMMAND_RULES[0]);

} // namespace

CommandParser::CommandParser()
{
}

ParseResult CommandParser::parse(const std::string& input) const
{
    std::string command;
    std::string parameters;

    const std::size_t first = input.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
    {
        return {
            ParserResult::INVALID,
            "",
            "",
            "Empty command"
        };
    }

    const std::size_t commandEnd =
        input.find_first_of(" \t\r\n", first);

    if (commandEnd == std::string::npos)
    {
        command = input.substr(first);
    }
    else
    {
        command = input.substr(first, commandEnd - first);

        const std::size_t parameterStart =
            input.find_first_not_of(" \t\r\n", commandEnd);

        if (parameterStart != std::string::npos)
        {
            parameters = input.substr(parameterStart);
        }
    }

    for (std::size_t i = 0; i < COMMAND_RULE_COUNT; ++i)
    {
        const CommandRule& rule = COMMAND_RULES[i];

        if (command != rule.command)
        {
            continue;
        }

        if (rule.parameterRegex == nullptr)
        {
            return {
                ParserResult::INVALID,
                command,
                "",
                "Parameter rule not implemented"
            };
        }

        const std::regex regex(rule.parameterRegex);

        if (!std::regex_match(parameters, regex))
        {
            return {
                ParserResult::INVALID,
                command,
                "",
                "Invalid parameters"
            };
        }

        return {
            ParserResult::VALID,
            command,
            input,
            ""
        };
    }

    return {
        ParserResult::INVALID,
        command,
        "",
        "Unknown command"
    };
}