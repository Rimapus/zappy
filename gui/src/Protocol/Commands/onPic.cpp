/*
 * EPITECH PROJECT, 2026
 * onPic.cpp
 * File description:
 * pic command handler
 */

#include "../../Components/Glow.hpp"
#include "../../Components/Levitate.hpp"
#include "../../Components/Player.hpp"
#include "../../Constants.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"
#include <SFML/Graphics/Color.hpp>

/**
 * @brief Handle pic: an incantation ritual started.
 *
 * @param cmd args: X, Y, L (required level), #n... (participating players)
 */
void Handler::onPic(const Command &cmd)
{
    int x;
    int y;
    int level;

    if (cmd._args.size() < 4) {
        Log::error() << "Invalid pic command: expected at least 4 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[0]);
        y = std::stoi(cmd._args[1]);
        level = std::stoi(cmd._args[2]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid pic command: invalid argument format: " << e.what() << "\n";
        return;
    }

    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid pic command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }
    if (level < 1 || level > 8) {
        Log::error() << "Invalid pic command: level out of range: " << level << "\n";
        return;
    }
    if (_gameState._incantations.find({static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)}) != _gameState._incantations.end()) {
        Log::error() << "Invalid pic command: an incantation is already running at (" << x << "," << y << ")\n";
        return;
    }

    std::vector<int> playerIds;
    for (std::size_t i = 3; i < cmd._args.size(); ++i) {
        int playerId;
        if (sscanf(cmd._args[i].c_str(), "#%d", &playerId) != 1) {
            Log::error() << "Invalid pic command: invalid player id format: " << cmd._args[i] << "\n";
            return;
        }
        if (_gameState._players.find(playerId) == _gameState._players.end()) {
            Log::error() << "Invalid pic command: unknown player " << playerId << "\n";
            return;
        }
        playerIds.push_back(playerId);
    }

    std::vector<std::uint32_t> participants;
    for (int id : playerIds) {
        Entity entity = _gameState._players.at(id);
        Player &player = _coordinator.getComponent<Player>(entity);
        sf::Color base = teamColor(_gameState._teams, player._team);

        _coordinator.addComponent(entity, Glow{base, sf::Color(255, 255, 255), INCANTATION_GLOW_SPEED, 0.0f});
        _coordinator.addComponent(entity, Levitate{LEVITATE_AMPLITUDE, LEVITATE_SPEED, 0.f, 0.f});
        participants.push_back(static_cast<std::uint32_t>(id));
    }
    _gameState._incantations[{static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)}] = participants;

    Log::info() << "Incantation started at (" << x << "," << y << ") level " << level << " with " << playerIds.size() << " player(s):";
    for (int id : playerIds) {
        Log::text() << " #" << id;
    }
    std::cout << LOG_RESET "\n";
}
