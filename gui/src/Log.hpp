/*
 * EPITECH PROJECT, 2026
 * Log.hpp
 * File description:
 * Colored logs
 */

#pragma once

#include <iostream>

#define LOG_RESET "\033[0m"
#define LOG_RED "\033[31m"
#define LOG_GREEN "\033[32m"
#define LOG_YELLOW "\033[33m"
#define LOG_CYAN "\033[36m"
#define LOG_MAGENTA "\033[35m"

/* @brief Colored logs */
namespace Log {

/* Errors in red */
inline std::ostream &error()
{
    return std::cerr << LOG_RED "[GUI]" LOG_RESET " ";
}

/* Game events in green */
inline std::ostream &info()
{
    return std::cout << LOG_GREEN "[GUI]" LOG_RESET " ";
}

/* Server warnings in yellow */
inline std::ostream &warn()
{
    return std::cerr << LOG_YELLOW "[GUI]" LOG_RESET " ";
}

/* Server messages in blue */
inline std::ostream &server()
{
    return std::cout << LOG_MAGENTA "[GUI]" LOG_RESET " ";
}

/* Continuation — no prefix, cyan */
inline std::ostream &text()
{
    return std::cout << LOG_CYAN;
}

} // namespace Log
