/*
 * EPITECH PROJECT, 2026
 * onPlv.cpp
 * File description:
 * plv command handler
 */

#include "../../Components/Player.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle plv: update a player's level.
 *
 * @param cmd args: #n, L (level 1-8)
 */
void Handler::onPlv(const Command &cmd)
{
    int playerId;
    int level;

    if (cmd._args.size() != 2) {
        Log::error() << "Invalid plv command: expected 2 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid plv command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }
    try {
        level = std::stoi(cmd._args[1]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid plv command: invalid argument format: " << e.what() << "\n";
        return;
    }

    if (level < 1 || level > 8) {
        Log::error() << "Invalid plv command: level out of range: " << level << "\n";
        return;
    }

    auto it = _gameState._players.find(playerId);
    if (it == _gameState._players.end()) {
        Log::error() << "Invalid plv command: unknown player " << playerId << "\n";
        return;
    }

    Player &player = _coordinator.getComponent<Player>(it->second);
    player._level = static_cast<std::uint32_t>(level);

    Log::info() << "Player " << playerId << " leveled up to " << level << "\n";
}
