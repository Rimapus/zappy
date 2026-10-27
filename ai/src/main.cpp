/*
** EPITECH PROJECT, 2026
** main.cpp
** File description:
** Zappy AI entry point — argument parsing and ZappyAI bootstrap
*/

#include "ZappyAi.hpp"

#include "policy/Policy.hpp"

#include <iostream>
#include <string>

namespace {

/**
 * @brief Print the usage message to stdout (help) or stderr (error).
 *
 * @param error  When true, prints to stderr and returns exit code 84.
 *               When false, prints to stdout and returns 0.
 * @return 0 on success, 84 on error.
 */
int show_usage(bool error)
{
    if (error) {
        std::cerr << "USAGE: ./zappy_ai -p port -n name -h machine [-s]\n";
    } else {
        std::cout << "USAGE: ./zappy_ai -p port -n name -h machine [-s]\n"
                     "  The hand-written scripted brain (climbs to level 8) is the\n"
                     "  default; -s selects it explicitly. Set ZAPPY_POLICY=model to\n"
                     "  use the experimental RL models instead.\n";
    }
    return error ? 84 : 0;
}

/**
 * @brief Parse command-line arguments and configure the AI object.
 *
 * Expected format (order-insensitive, pairs of flag + value):
 * @code
 * -h <host>  -p <port>  -n <team>
 * @endcode
 *
 * @p host and @p team are mandatory; @p port defaults to DEFAULT_PORT.
 *
 * @param argc  Argument count from main().
 * @param argv  Argument vector from main().
 * @param ai    AI object to configure via set_host / set_port / set_team.
 * @return 0 on success, 84 if an unknown flag is encountered or a mandatory
 *         argument is missing.
 */
int parse_arguments(int argc, char **argv, zappy::ZappyAI &ai)
{
    std::string host = "localhost"; // subject: "-h machine ... localhost by default"
    std::string port = DEFAULT_PORT;
    std::string team;
    bool scripted = false;

    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        if (arg == "-s") {
            scripted = true;
            continue;
        }
        if (i + 1 >= argc) {
            return 84;
        }
        if (arg == "-h") {
            host = argv[++i];
        } else if (arg == "-p") {
            port = argv[++i];
        } else if (arg == "-n") {
            team = argv[++i];
        } else {
            return 84;
        }
    }

    if (team.empty()) { // -n is mandatory; -p and -h have defaults
        return 84;
    }

    ai.set_host(host);
    ai.set_port(port);
    ai.set_team(team);
    if (scripted) {
        ai.set_policy_kind(ai::PolicyKind::Scripted);
    }
    return 0;
}

} // namespace

/**
 * @brief Program entry point.
 *
 * Parses arguments, constructs the AI, and delegates to ZappyAI::run().
 * All exceptions thrown by the network layer are caught here and reported on
 * stderr with exit code 84.
 *
 * @param argc  Argument count.
 * @param argv  Argument vector.
 * @return 0 on success, 84 on error.
 */
int main(int argc, char **argv)
{
    zappy::ZappyAI ai("localhost", DEFAULT_PORT, "team");

    if (argc == 2 && std::string(argv[1]) == "--help") {
        return show_usage(false);
    }

    if (argc < 5 || argc > 8) {
        return show_usage(true);
    }

    if (parse_arguments(argc, argv, ai) == 84) {
        return show_usage(true);
    }

    try {
        ai.run();
    } catch (const ai::SocketClosed &e) {
        std::cerr << "Connection closed by server: " << e.what() << "\n";
        return 84;
    } catch (const ai::SocketTimeout &e) {
        std::cerr << "Server timed out: " << e.what() << "\n";
        return 84;
    } catch (const ai::SocketException &e) {
        std::cerr << "Socket error: " << e.what() << "\n";
        return 84;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 84;
    }
    return 0;
}
