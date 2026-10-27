/*
 * EPITECH PROJECT, 2026
 * onEbo.cpp
 * File description:
 * ebo command handler
 */

#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../../Systems/ParticleSystem/ParticleSystem.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle ebo: a player hatched from an egg (egg entity removed).
 *
 * @param cmd args: #e (egg id)
 */
void Handler::onEbo(const Command &cmd)
{
    int eggId;

    if (cmd._args.size() != 1) {
        Log::error() << "Invalid ebo command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &eggId) != 1) {
        Log::error() << "Invalid ebo command: invalid egg id format: " << cmd._args[0] << "\n";
        return;
    }

    auto it = _gameState._eggs.find(eggId);
    if (it == _gameState._eggs.end()) {
        Log::error() << "Invalid ebo command: unknown egg " << eggId << "\n";
        return;
    }

    Transform &t = _coordinator.getComponent<Transform>(it->second);
    float cx = static_cast<float>(t._x) + EGG_W * ENTITY_SCALE / 2.f;
    float cy = static_cast<float>(t._y) + EGG_H * ENTITY_SCALE / 2.f;

    _coordinator.destroyEntity(it->second);
    _gameState._eggs.erase(it);

    ParticleSystem::spawnTeleport(_coordinator, cx, cy, false, sf::Color::White);
    Log::info() << "Egg " << eggId << " hatched\n";
}
