/*
** EPITECH PROJECT, 2026
** Main.cpp
** File description:
** Main file for the zappy_server
*/

#include "Main.hpp"
#include "Loop.hpp"
#include <cstdint>
#include <cstdio>
#include <exception>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

static bool isOption(std::string_view argument)
{
    return argument.starts_with('-') && argument.size() > 1;
}

static bool parsePortOption(std::size_t &index, const std::vector<std::string_view> &arguments, std::uint16_t &value)
{
    try {
        std::size_t parsed = 0;
        std::string rawArgument(arguments.at(++index));
        unsigned long rawValue = std::stoul(rawArgument, &parsed);

        if (parsed != rawArgument.size() || rawValue == 0 || rawValue > 65535UL) {
            return false;
        }

        value = static_cast<std::uint16_t>(rawValue);
    } catch (const std::exception &) {
        return false;
    }
    return true;
}

static bool parseSizedOption(std::size_t &index, const std::vector<std::string_view> &arguments, std::size_t &value)
{
    try {
        std::size_t parsed = 0;
        std::string rawArgument(arguments.at(++index));
        unsigned long rawValue = std::stoul(rawArgument, &parsed);

        if (parsed != rawArgument.size() || rawValue == 0) {
            return false;
        }

        value = static_cast<std::size_t>(rawValue);
    } catch (const std::exception &) {
        return false;
    }
    return true;
}

static bool parseTeamNames(std::size_t &index, const std::vector<std::string_view> &arguments, Config &config)
{
    try {
        while (!isOption(arguments.at(index + 1))) {
            auto name = arguments.at(++index);
            if (name == "GRAPHIC") {
                return false;
            }
            config._teamNames.emplace_back(name);
        }
    } catch (const std::out_of_range &) {
        return !config._teamNames.empty();
    }
    return !config._teamNames.empty();
}

static bool parseArguments(const std::vector<std::string_view> &arguments, Config &config)
{
    config = Config{};

    for (std::size_t i = 1; i < arguments.size(); i++) {
        if (arguments.at(i) == "-p") {
            if (config._port != 0 || !parsePortOption(i, arguments, config._port)) {
                return false;
            }
            continue;
        }
        if (arguments.at(i) == "-x") {
            if (config._width != 0 || !parseSizedOption(i, arguments, config._width)) {
                return false;
            }
            continue;
        }
        if (arguments.at(i) == "-y") {
            if (config._height != 0 || !parseSizedOption(i, arguments, config._height)) {
                return false;
            }
            continue;
        }
        if (arguments.at(i) == "-n") {
            if (!config._teamNames.empty() || !parseTeamNames(i, arguments, config)) {
                return false;
            }
            continue;
        }
        if (arguments.at(i) == "-c") {
            if (config._clientsNumber != 0 || !parseSizedOption(i, arguments, config._clientsNumber)) {
                return false;
            }
            continue;
        }
        if (arguments.at(i) == "-f") {
            if (config._frequency != 0 || !parseSizedOption(i, arguments, config._frequency)) {
                return false;
            }
            continue;
        }
        return false;
    }

    return config._port != 0 && config._width != 0 && config._height != 0 && !config._teamNames.empty() && config._clientsNumber != 0 && config._frequency != 0;
}

static void displayUsage(std::FILE *stream)
{
    std::println(stream,
                 "USAGE: ./zappy_server -p port -x width -y height -n name1 name2 ... -c clientsNb -f freq\n"
                 "  -p port              port number\n"
                 "  -x width             width of the world\n"
                 "  -y height            height of the world\n"
                 "  -n name1 name2 ...   name of the team\n"
                 "  -c clientsNb         number of initial clients per team\n"
                 "  -f freq              reciprocal of time unit for execution of actions");
}

int main(int argc, char *argv[])
{
    std::vector<std::string_view> arguments;
    arguments.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; i++) {
        arguments.emplace_back(argv[i]);
    }

    if (arguments.size() == 2 && (arguments.at(1) == "-h" || arguments.at(1) == "--help")) {
        displayUsage(stdout);
        return 0;
    }

    Config config;
    if (!parseArguments(arguments, config)) {
        displayUsage(stderr);
        return 84;
    }

    return server::run(config);
}
