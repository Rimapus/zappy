/*
** EPITECH PROJECT, 2026
** Logger.cpp
** File description:
** Logger class
*/

#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <format>
#include <iomanip>
#include <print>
#include <sstream>
#include <string>

namespace server {

static std::string currentTimestamp(void)
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&time);

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%F %T");
    return stream.str();
}

void Logger::writeLine(const std::string &level, const std::string &message)
{
    const std::string line = std::format("[{}] [{}] {}", currentTimestamp(), level, message);

    std::println(stderr, "{}", line);
}

}
