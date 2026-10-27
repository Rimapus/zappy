/*
 * EPITECH PROJECT, 2026
 * onPgt.cpp
 * File description:
 * pgt command handler
 */

#include "../../Components/Inventory.hpp"
#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle pgt: a player picked up a resource from its tile.
 *
 * @param cmd args: #n, i (resource index 0-6)
 */
void Handler::onPgt(const Command &cmd)
{
    int playerId;
    int resourceIndex;

    if (cmd._args.size() != 2) {
        Log::error() << "Invalid pgt command: expected 2 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid pgt command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }
    try {
        resourceIndex = std::stoi(cmd._args[1]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid pgt command: invalid argument format: " << e.what() << "\n";
        return;
    }
    if (resourceIndex < 0 || resourceIndex > 6) {
        Log::error() << "Invalid pgt command: resource index out of range: " << resourceIndex << "\n";
        return;
    }

    auto it = _gameState._players.find(playerId);
    if (it == _gameState._players.end()) {
        Log::error() << "Invalid pgt command: unknown player " << playerId << "\n";
        return;
    }

    Inventory &inv = _coordinator.getComponent<Inventory>(it->second);
    inv._resources[resourceIndex]++;

    Log::info() << "Player " << playerId << " picked up " << RESOURCE_NAMES[resourceIndex] << "\n";
}
