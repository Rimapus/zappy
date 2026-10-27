/*
** EPITECH PROJECT, 2026
** Logger.hpp
** File description:
** Logger class
*/

#pragma once

#include <format>
#include <string>
#include <utility>

namespace server {

class Logger
{
public:
    template <typename... Args>
    static void info(std::format_string<Args...> format, Args &&...args)
    {
        writeLine("INFO", std::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    static void error(std::format_string<Args...> format, Args &&...args)
    {
        writeLine("ERROR", std::format(format, std::forward<Args>(args)...));
    }

private:
    static void writeLine(const std::string &level, const std::string &message);
};

}
