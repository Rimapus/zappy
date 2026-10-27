/*
 * EPITECH PROJECT, 2026
 * onTna.cpp
 * File description:
 * tna command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"
#include <algorithm>

/**
 * @brief Handle tna: register a team name.
 *
 * @param cmd args: N (team name)
 */
void Handler::onTna(const Command &cmd)
{
    if (cmd._args.size() != 1) {
        Log::error() << "Invalid TNA command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    auto const &teams = _gameState._teams;

    if (std::find(teams.begin(), teams.end(), cmd._args[0]) != teams.end()) {
        Log::error() << "Invalid TNA command: team \"" << cmd._args[0] << "\" already registered\n";
        return;
    }
    _gameState._teams.push_back(cmd._args[0]);
    Log::info() << "Registered team: " << cmd._args[0] << "\n";
}
