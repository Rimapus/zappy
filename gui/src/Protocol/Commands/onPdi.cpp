/*
 * EPITECH PROJECT, 2026
 * onPdi.cpp
 * File description:
 * pdi command handler
 */

#include "../../Components/Glow.hpp"
#include "../../Components/Lifetime.hpp"
#include "../../Components/Player.hpp"
#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../../Systems/AnimatorSystem/AnimatorSystem.hpp"
#include "../Handler.hpp"
#include "../PlayerOrientation.hpp"

/**
 * @brief Handle pdi: a player died.
 *
 * The player is detached from the game state right away, but its entity is
 * kept alive a short while (with a Lifetime) so the death animation can play
 * out before the LifetimeSystem destroys it.
 *
 * @param cmd args: #n (player id)
 */
void Handler::onPdi(const Command &cmd)
{
    int playerId;

    if (cmd._args.size() != 1) {
        Log::error() << "Invalid pdi command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid pdi command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }

    auto it = _gameState._players.find(playerId);
    if (it == _gameState._players.end()) {
        Log::error() << "Invalid pdi command: unknown player " << playerId << "\n";
        return;
    }

    Entity entity = it->second;
    Player &player = _coordinator.getComponent<Player>(entity);

    if (_coordinator.hasComponent<Glow>(entity)) {
        _coordinator.removeComponent<Glow>(entity);
    }
    AnimatorSystem::play(_coordinator, entity, "death_" + orientName(static_cast<int>(player._orientation)));
    _coordinator.addComponent(entity, Lifetime{DEATH_ANIM_DURATION});
    _gameState._players.erase(it);

    Log::info() << "Player " << playerId << " died\n";
}
