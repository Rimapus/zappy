/*
 * EPITECH PROJECT, 2026
 * onPex.cpp
 * File description:
 * pex command handler
 */

#include "../../Components/Player.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Log.hpp"
#include "../../Systems/AnimatorSystem/AnimatorSystem.hpp"
#include "../../Systems/ParticleSystem/ParticleSystem.hpp"
#include "../Handler.hpp"
#include "../PlayerOrientation.hpp"
#include <numbers>

/**
 * @brief Handle pex: a player expelled all others from its tile.
 *
 * @param cmd args: #n (expelling player id)
 */
void Handler::onPex(const Command &cmd)
{
    int playerId;

    if (cmd._args.size() != 1) {
        Log::error() << "Invalid pex command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid pex command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }
    auto it = _gameState._players.find(playerId);
    if (it == _gameState._players.end()) {
        Log::error() << "Invalid pex command: unknown player " << playerId << "\n";
        return;
    }

    Entity entity = it->second;
    Player &player = _coordinator.getComponent<Player>(entity);
    std::string direction = orientName(static_cast<int>(player._orientation));

    AnimatorSystem::playOnce(_coordinator, entity, "hurt_" + direction, "idle_" + direction);

    static constexpr float HALF_PI = std::numbers::pi_v<float> / 2.f;
    static constexpr float ORIENT_ANGLES[5] = {0.f, -HALF_PI, 0.f, HALF_PI, std::numbers::pi_v<float>};
    float dirAngle = ORIENT_ANGLES[static_cast<int>(player._orientation)];

    Transform &t = _coordinator.getComponent<Transform>(entity);
    float cx = static_cast<float>(t._x) + PLAYER_FRAME * ENTITY_SCALE / 2.f;
    float cy = static_cast<float>(t._y) + PLAYER_CONTENT_CENTER_Y * ENTITY_SCALE;
    sf::Color color = teamColor(_gameState._teams, player._team);
    ParticleSystem::spawnShockwave(_coordinator, cx, cy, dirAngle, color);

    Log::info() << "Player " << playerId << " expelled players from its tile\n";
}
