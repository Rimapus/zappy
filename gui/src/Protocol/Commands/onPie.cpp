/*
 * EPITECH PROJECT, 2026
 * onPie.cpp
 * File description:
 * pie command handler
 */

#include "../../Components/Glow.hpp"
#include "../../Components/Levitate.hpp"
#include "../../Components/Player.hpp"
#include "../../Components/Sprite.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"
#include <SFML/Graphics/Color.hpp>

/**
 * @brief Handle pie: an incantation ritual ended.
 *
 * @param cmd args: X, Y, R (result: 0=failure, 1=success)
 */
void Handler::onPie(const Command &cmd)
{
    int x;
    int y;
    int result;

    if (cmd._args.size() != 3) {
        Log::error() << "Invalid pie command: expected 3 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[0]);
        y = std::stoi(cmd._args[1]);
        result = std::stoi(cmd._args[2]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid pie command: invalid argument format: " << e.what() << "\n";
        return;
    }
    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid pie command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }
    if (result != 0 && result != 1) {
        Log::error() << "Invalid pie command: result must be 0 or 1, got " << result << "\n";
        return;
    }

    auto incantation = _gameState._incantations.find({static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)});
    if (incantation == _gameState._incantations.end()) {
        Log::error() << "Invalid pie command: no incantation running at (" << x << "," << y << ")\n";
        return;
    }
    for (std::uint32_t playerId : incantation->second) {
        auto it = _gameState._players.find(playerId);
        if (it == _gameState._players.end()) {
            continue;
        }
        _coordinator.removeComponent<Glow>(it->second);
        _coordinator.removeComponent<Levitate>(it->second);

        Player &player = _coordinator.getComponent<Player>(it->second);
        Sprite &sprite = _coordinator.getComponent<Sprite>(it->second);
        if (sprite._sprite.has_value() == true) {
            sprite._sprite->setColor(teamColor(_gameState._teams, player._team));
        }
    }
    _gameState._incantations.erase(incantation);

    if (result == 1) {
        Log::info() << "Incantation at (" << x << "," << y << ") succeeded\n";
    } else {
        Log::info() << "Incantation at (" << x << "," << y << ") failed\n";
    }
}
