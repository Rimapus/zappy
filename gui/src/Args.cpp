/*
 * EPITECH PROJECT, 2026
 * Args.cpp
 * File description:
 * command line arguments parsing for the GUI
 */

#include "Args.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

/**
 * @brief Parse the GUI command line (`-p port -h machine`).
 *
 * @param argc The argument count
 * @param argv The argument vector
 * @param out  Receives the parsed options on success
 *
 * @return true on success, false on a malformed command line.
 */
bool Args::parse(int argc, char *argv[], GuiArgs &out)
{
    long port = 0;

    for (int i = 1; i < argc; i += 2) {
        std::string flag = argv[i];

        if (i + 1 >= argc) {
            return false;
        }
        std::string value = argv[i + 1];
        if (flag == "-p") {
            char *end = nullptr;
            port = std::strtol(value.c_str(), &end, 10);
            if (*end != '\0' || port <= 0 || port > 65535) {
                return false;
            }
            out._port = static_cast<std::uint16_t>(port);
        } else if (flag == "-h") {
            out._host = value;
        } else {
            return false;
        }
    }
    return out._port != 0;
}

/**
 * @brief Print the usage line to stderr.
 *
 * @param program The program name (argv[0])
 */
void Args::usage(const char *program)
{
    std::cerr << "USAGE: " << program << " -p port -h machine\n"
              << "  -p port      port number of the server\n"
              << "  -h machine   hostname of the server (default: localhost)\n";
}
