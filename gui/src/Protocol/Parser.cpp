/*
** EPITECH PROJECT, 2026
** gui/src/Protocol/Parser
** File description:
** Parser source file
*/

#include "Parser.hpp"
#include <sstream>
#include <unordered_map>

/* @brief Map of command strings to CommandType enums */
static const std::unordered_map<std::string, CommandType> COMMAND_MAP = {
    {"msz", CommandType::MSZ},
    {"bct", CommandType::BCT},
    {"mct", CommandType::MCT},
    {"tna", CommandType::TNA},
    {"pnw", CommandType::PNW},
    {"ppo", CommandType::PPO},
    {"plv", CommandType::PLV},
    {"pin", CommandType::PIN},
    {"pex", CommandType::PEX},
    {"pbc", CommandType::PBC},
    {"pic", CommandType::PIC},
    {"pie", CommandType::PIE},
    {"pfk", CommandType::PFK},
    {"pdr", CommandType::PDR},
    {"pgt", CommandType::PGT},
    {"pdi", CommandType::PDI},
    {"enw", CommandType::ENW},
    {"ebo", CommandType::EBO},
    {"edi", CommandType::EDI},
    {"sgt", CommandType::SGT},
    {"sst", CommandType::SST},
    {"seg", CommandType::SEG},
    {"smg", CommandType::SMG},
    {"suc", CommandType::SUC},
    {"sbp", CommandType::SBP},
};

/**
 * @brief Split a line into whitespace-delimited tokens.
 *
 * @param line The raw input line
 * @return Vector of tokens
 */
std::vector<std::string> Parser::split(const std::string &line)
{
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * @brief Parse a raw protocol line into a Command.
 *
 * @param line The raw input line (without trailing newline)
 * @return Parsed Command with type and arguments
 */
Command Parser::parse(const std::string &line)
{
    std::vector<std::string> tokens = Parser::split(line);

    if (tokens.empty()) {
        return {CommandType::UNKNOWN, {}};
    }

    auto it = COMMAND_MAP.find(tokens[0]);

    if (it == COMMAND_MAP.end()) {
        return {CommandType::UNKNOWN, tokens};
    }
    return {it->second, std::vector<std::string>(tokens.begin() + 1, tokens.end())};
}
