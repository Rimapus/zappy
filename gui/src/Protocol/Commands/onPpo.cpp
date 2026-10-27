/*
 * EPITECH PROJECT, 2026
 * onPpo.cpp
 * File description:
 * ppo command handler
 */

#include "../../Components/Movement.hpp"
#include "../../Components/Offsets.hpp"
#include "../../Components/Player.hpp"
#include "../../Components/TileAnchor.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Interpolation.hpp"
#include "../../Log.hpp"
#include "../../Systems/AnimatorSystem/AnimatorSystem.hpp"
#include "../../Systems/ParticleSystem/ParticleSystem.hpp"
#include "../Handler.hpp"
#include "../PlayerOrientation.hpp"

/**
 * @brief Handle ppo: update a player's position and orientation.
 *
 * @param cmd args: #n, X, Y, O (orientation 1-4)
 */
void Handler::onPpo(const Command &cmd)
{
    int playerId;
    int x;
    int y;
    int orientation;

    if (cmd._args.size() != 4) {
        Log::error() << "Invalid ppo command: expected 4 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid ppo command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[1]);
        y = std::stoi(cmd._args[2]);
        orientation = std::stoi(cmd._args[3]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid ppo command: invalid argument format: " << e.what() << "\n";
        return;
    }

    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid ppo command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }
    if (orientation < 1 || orientation > 4) {
        Log::error() << "Invalid ppo command: orientation out of range: " << orientation << "\n";
        return;
    }

    auto it = _gameState._players.find(playerId);
    if (it == _gameState._players.end()) {
        Log::error() << "Invalid ppo command: unknown player " << playerId << "\n";
        return;
    }

    Entity entity = it->second;

    Transform &transform = _coordinator.getComponent<Transform>(entity);
    Movement &movement = _coordinator.getComponent<Movement>(entity);
    Player &player = _coordinator.getComponent<Player>(entity);
    Offsets &offsets = _coordinator.getComponent<Offsets>(entity);

    std::int32_t newX = _gameState._tileOriginX + x * WORLD_TILE_PX + PLAYER_SPRITE_OFFSET_X + static_cast<std::int32_t>(offsets._x[0]);
    std::int32_t newY = _gameState._tileOriginY + y * WORLD_TILE_PX + PLAYER_SPRITE_OFFSET_Y + static_cast<std::int32_t>(offsets._y[0]);
    bool moved = (transform._x != newX || transform._y != newY);
    std::string direction = orientName(orientation);

    std::int32_t prevPixelX = (movement._progress >= 1.0f) ? transform._x : movement._targetX;
    std::int32_t prevPixelY = (movement._progress >= 1.0f) ? transform._y : movement._targetY;
    bool isWrap = std::abs(newX - prevPixelX) > WORLD_TILE_PX || std::abs(newY - prevPixelY) > WORLD_TILE_PX;

    TileAnchor &anchor = _coordinator.getComponent<TileAnchor>(entity);

    player._orientation = static_cast<std::uint32_t>(orientation);
    anchor._x = static_cast<std::uint32_t>(x);
    anchor._y = static_cast<std::uint32_t>(y);

    if (moved && !isWrap) {
        startMove(transform, movement, newX, newY, MOVE_INTERPOLATION_DURATION);
        AnimatorSystem::playOnce(_coordinator, entity, "walk_" + direction, "idle_" + direction);
    } else {
        if (isWrap) {
            constexpr float halfW = PLAYER_FRAME * ENTITY_SCALE / 2.f;
            constexpr float centerY = PLAYER_CONTENT_CENTER_Y * ENTITY_SCALE;
            float oldCx = static_cast<float>(transform._x) + halfW;
            float oldCy = static_cast<float>(transform._y) + centerY;
            float newCx = static_cast<float>(newX) + halfW;
            float newCy = static_cast<float>(newY) + centerY;
            sf::Color color = teamColor(_gameState._teams, player._team);
            ParticleSystem::spawnTeleport(_coordinator, oldCx, oldCy, false, color);
            ParticleSystem::spawnTeleport(_coordinator, newCx, newCy, true, color);
        }
        transform._x = newX;
        transform._y = newY;
        movement._progress = 1.0f;
        AnimatorSystem::play(_coordinator, entity, "idle_" + direction);
    }

    Log::info() << "Player " << playerId << " moved to (" << x << "," << y
                << ") facing " << orientation << "\n";
}
