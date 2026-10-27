/*
 * EPITECH PROJECT, 2026
 * onEnw.cpp
 * File description:
 * enw command handler
 */

#include "../../Components/Egg.hpp"
#include "../../Components/Player.hpp"
#include "../../Components/Sprite.hpp"
#include "../../Components/TileAnchor.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Log.hpp"
#include "../../Resources/TextureManager.hpp"
#include "../Handler.hpp"
#include <SFML/Graphics/Sprite.hpp>

/**
 * @brief Handle enw: an egg was laid and must be spawned as an entity.
 *
 * @param cmd args: #e (egg id), #n (laying player id), X, Y
 */
void Handler::onEnw(const Command &cmd)
{
    int eggId;
    int playerId;
    int x;
    int y;

    if (cmd._args.size() != 4) {
        Log::error() << "Invalid enw command: expected 4 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &eggId) != 1) {
        Log::error() << "Invalid enw command: invalid egg id format: " << cmd._args[0] << "\n";
        return;
    }
    if (sscanf(cmd._args[1].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid enw command: invalid player id format: " << cmd._args[1] << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[2]);
        y = std::stoi(cmd._args[3]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid enw command: invalid argument format: " << e.what() << "\n";
        return;
    }
    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid enw command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }
    if (playerId != -1 && _gameState._players.find(playerId) == _gameState._players.end()) {
        Log::warn() << "enw command: unknown player " << playerId << " (may have disconnected)\n";
    }
    if (_gameState._eggs.count(eggId)) {
        Log::error() << "Invalid enw command: egg " << eggId << " already exists\n";
        return;
    }

    TextureManager &textures = _coordinator.getResource<TextureManager>();
    Entity eggEntity = _coordinator.createEntity();

    sf::Sprite sprite(textures.get("egg"));
    auto layingPlayer = _gameState._players.find(playerId);
    if (layingPlayer != _gameState._players.end()) {
        Player &player = _coordinator.getComponent<Player>(layingPlayer->second);
        sprite.setColor(teamColor(_gameState._teams, player._team));
    }
    _coordinator.addComponent(eggEntity, Transform{
                                             _gameState._tileOriginX + x * WORLD_TILE_PX + EGG_SPRITE_OFFSET_X,
                                             _gameState._tileOriginY + y * WORLD_TILE_PX + EGG_SPRITE_OFFSET_Y,
                                             Z_EGG});
    _coordinator.addComponent(eggEntity, Sprite{sprite});
    _coordinator.addComponent(eggEntity, Egg{static_cast<std::uint32_t>(eggId), static_cast<std::uint32_t>(playerId)});
    _coordinator.addComponent(eggEntity, TileAnchor{static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)});

    _gameState._eggs[eggId] = eggEntity;
    Log::info() << "Egg " << eggId << " laid by player " << playerId << " at (" << x << "," << y << ")\n";
}
