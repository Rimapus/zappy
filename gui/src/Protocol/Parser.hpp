/*
** EPITECH PROJECT, 2026
** gui/src/Protocol/Parser
** File description:
** Parser header file
*/

#pragma once

#include "Commands.hpp"
#include <string>
#include <vector>

/* @brief Parser class */
class Parser
{
public:
    /**
     * @brief Parse a line into a Command struct
     *
     * @param line The line to parse
     * @return Command The parsed command with its arguments
     */
    static Command parse(const std::string &line);

private:
    /**
     * @brief Split a line into tokens
     *
     * @param line The line to split
     * @return std::vector<std::string> The tokens extracted from the line
     */
    static std::vector<std::string> split(const std::string &line);
};
