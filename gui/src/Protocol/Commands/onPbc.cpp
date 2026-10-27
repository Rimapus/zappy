/*
 * EPITECH PROJECT, 2026
 * onPbc.cpp
 * File description:
 * pbc command handler
 */

#include "../../Components/Player.hpp"
#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"
#include <cstdio>

/**
 * @brief Handle pbc: a player broadcast a message.
 *
 * @param cmd args: #n, M (message text)
 */
void Handler::onPbc(const Command &cmd)
{
    if (cmd._args.size() < 2) {
        Log::error() << "Invalid PBC command: expected at least 2 arguments, got " << cmd._args.size()
                     << "\n";
        return;
    }

    int playerIdRaw;
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerIdRaw) != 1) {
        Log::error() << "Invalid PBC command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }

    std::uint32_t playerId = static_cast<std::uint32_t>(playerIdRaw);
    std::string team;
    auto it = _gameState._players.find(playerId);

    if (it != _gameState._players.end()) {
        team = _coordinator.getComponent<Player>(it->second)._team;
    }

    std::string message = cmd._args[1];
    for (std::size_t i = 2; i < cmd._args.size(); ++i) {
        message += ' ' + cmd._args[i];
    }
    _gameState._chat.push_back({playerId, team, message, 0.0f});
    if (_gameState._chat.size() > CHAT_MAX_MESSAGES) {
        _gameState._chat.pop_front();
    }
    Log::server() << "Player " << cmd._args[0] << " broadcasts: " << message << "\n";
}
