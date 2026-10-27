/*
** EPITECH PROJECT, 2026
** StringUtils.hpp
** File description:
** Shared string helpers
*/

#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace server {

static inline std::string toLowerCase(std::string string)
{
    std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return string;
}

}
