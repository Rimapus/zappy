/*
 * EPITECH PROJECT, 2026
 * onSeg.cpp
 * File description:
 * seg command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"
#include <algorithm>

/**
 * @brief Handle seg: the game is over.
 *
 * @param cmd args: N (winning team name)
 */
void Handler::onSeg(const Command &cmd)
{
    if (cmd._args.size() != 1) {
        Log::error() << "Invalid SEG command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    auto const &teams = _gameState._teams;
    if (!teams.empty() && std::find(teams.begin(), teams.end(), cmd._args[0]) == teams.end()) {
        Log::warn() << "SEG command: unknown team \"" << cmd._args[0] << "\" — game over anyway\n";
    }
    _gameState._gameOver = true;
    _gameState._winnerTeam = cmd._args[0];
    Log::info() << "Game over! Winning team: " << cmd._args[0] << "\n";
}
