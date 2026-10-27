/*
 * EPITECH PROJECT, 2026
 * onPin.cpp
 * File description:
 * pin command handler
 */

#include "../../Components/Inventory.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle pin: update a player's inventory.
 *
 * @param cmd args: #n, X, Y, q0..q6 (resource quantities)
 */
void Handler::onPin(const Command &cmd)
{
    int playerId;
    int x;
    int y;
    std::array<std::uint32_t, 7> inventory = {};

    if (cmd._args.size() != 10) {
        Log::error() << "Invalid pin command: expected 10 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid pin command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[1]);
        y = std::stoi(cmd._args[2]);
        for (int i = 0; i < 7; ++i) {
            inventory[i] = static_cast<std::uint32_t>(std::stoul(cmd._args[3 + i]));
        }
    } catch (const std::exception &e) {
        Log::error() << "Invalid pin command: invalid argument format: " << e.what() << "\n";
        return;
    }

    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid pin command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }

    auto it = _gameState._players.find(playerId);
    if (it == _gameState._players.end()) {
        Log::error() << "Invalid pin command: unknown player " << playerId << "\n";
        return;
    }

    Entity entity = it->second;
    Transform &transform = _coordinator.getComponent<Transform>(entity);
    Inventory &inv = _coordinator.getComponent<Inventory>(entity);

    transform._x = _gameState._tileOriginX + x * WORLD_TILE_PX + PLAYER_SPRITE_OFFSET_X;
    transform._y = _gameState._tileOriginY + y * WORLD_TILE_PX + PLAYER_SPRITE_OFFSET_Y;
    inv._resources = inventory;

    Log::info() << "Player " << playerId << " inventory at (" << x << "," << y << "):";
    for (int i = 0; i < 7; ++i) {
        Log::text() << " " << RESOURCE_NAMES[i] << "=" << inventory[i];
    }
    std::cout << LOG_RESET "\n";
}
