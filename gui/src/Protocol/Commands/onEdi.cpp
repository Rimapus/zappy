/*
 * EPITECH PROJECT, 2026
 * onEdi.cpp
 * File description:
 * edi command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle edi: an egg died — destroy the egg entity.
 *
 * @param cmd args: #e (egg id)
 */
void Handler::onEdi(const Command &cmd)
{
    int eggId;

    if (cmd._args.size() != 1) {
        Log::error() << "Invalid edi command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &eggId) != 1) {
        Log::error() << "Invalid edi command: invalid egg id format: " << cmd._args[0] << "\n";
        return;
    }

    auto it = _gameState._eggs.find(eggId);
    if (it == _gameState._eggs.end()) {
        Log::error() << "Invalid edi command: unknown egg " << eggId << "\n";
        return;
    }

    _coordinator.destroyEntity(it->second);
    _gameState._eggs.erase(it);

    Log::info() << "Egg " << eggId << " died\n";
}
