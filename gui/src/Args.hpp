/*
 * EPITECH PROJECT, 2026
 * Args.hpp
 * File description:
 * command line arguments parsing for the GUI
 */

#pragma once

#include <cstdint>
#include <string>

/* @brief Parsed command line options for the GUI client */
struct GuiArgs
{
    std::string _host = "localhost";
    std::uint16_t _port = 0;
};

/* @brief Parser for the `-p port -h machine` command line */
class Args
{
public:
    /**
     * @brief Parse the GUI command line (`-p port -h machine`).
     *
     * @param argc The argument count
     * @param argv The argument vector
     * @param out  Receives the parsed options on success
     *
     * @return true on success, false on a malformed command line.
     */
    static bool parse(int argc, char *argv[], GuiArgs &out);

    /**
     * @brief Print the usage line.
     *
     * @param program The program name (argv[0])
     */
    static void usage(const char *program);
};
