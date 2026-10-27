/*
 * EPITECH PROJECT, 2026
 * onBct.cpp
 * File description:
 * bct command handler
 */

#include "../../Components/Inventory.hpp"
#include "../../Components/Offsets.hpp"
#include "../../Components/Tile.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"
#include <random>

/**
 * @brief Handle bct: update the resource content of a tile.
 *
 * @param cmd args: X, Y, q0..q6 (resource quantities)
 */
void Handler::onBct(const Command &cmd)
{
    int x;
    int y;
    std::array<std::uint32_t, 7> resources = {};

    if (cmd._args.size() != 9) {
        Log::error() << "Invalid bct command: expected 9 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[0]);
        y = std::stoi(cmd._args[1]);
        for (int i = 0; i < 7; ++i) {
            resources[i] = static_cast<std::uint32_t>(std::stoul(cmd._args[2 + i]));
        }
    } catch (const std::exception &e) {
        Log::error() << "Invalid bct command: invalid argument format: " << e.what() << "\n";
        return;
    }

    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid bct command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }

    auto key = std::make_pair(static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y));
    auto it = _gameState._tiles.find(key);

    if (it == _gameState._tiles.end()) {
        static std::mt19937 rng{std::random_device{}()};
        Offsets offsets;

        for (int i = 0; i < 7; ++i) {
            float spriteHalf = (i == 0 ? RESSOURCES_SIZE * FOOD_SCALE : RESSOURCES_SIZE * MINERAL_SCALE) / 2.f;
            float bound = static_cast<float>(WORLD_TILE_PX) / 2.f - spriteHalf;
            std::uniform_real_distribution<float> dist(-bound, bound);

            offsets._x[i] = dist(rng);
            offsets._y[i] = dist(rng);
        }

        Entity tileEntity = _coordinator.createEntity();
        _coordinator.addComponent(tileEntity, Transform{
                                                  _gameState._tileOriginX + x * WORLD_TILE_PX,
                                                  _gameState._tileOriginY + y * WORLD_TILE_PX,
                                                  0});
        _coordinator.addComponent(tileEntity, Inventory{resources});
        _coordinator.addComponent(tileEntity, Tile{});
        _coordinator.addComponent(tileEntity, offsets);
        _gameState._tiles[key] = tileEntity;
    } else {
        Inventory &inv = _coordinator.getComponent<Inventory>(it->second);
        inv._resources = resources;
    }

    Log::info() << "Tile (" << x << "," << y << "):";
    for (int i = 0; i < 7; ++i) {
        if (resources[i] > 0) {
            Log::text() << " " << RESOURCE_NAMES[i] << "=" << resources[i];
        }
    }
    std::cout << LOG_RESET "\n";
}
