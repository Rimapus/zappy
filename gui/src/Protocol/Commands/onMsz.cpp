/*
 * EPITECH PROJECT, 2026
 * onMsz.cpp
 * File description:
 * msz command handler
 */

#include "../../Log.hpp"
#include "../../World/WorldGenerator.hpp"
#include "../Handler.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

/**
 * @brief Handle msz: set the map dimensions.
 *
 * @param cmd args: X (width), Y (height)
 */
void Handler::onMsz(const Command &cmd)
{
    std::size_t expectedArgs = 2;
    std::uint32_t width;
    std::uint32_t height;

    if (cmd._args.size() != expectedArgs) {
        Log::error() << "Invalid MSZ command: expected " << expectedArgs << " arguments, got "
                     << cmd._args.size() << "\n";
        return;
    }
    try {
        width = std::stoul(cmd._args[0]);
        height = std::stoul(cmd._args[1]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid MSZ command: arguments must be unsigned integers\n";
        return;
    }
    if (width == 0 || height == 0) {
        Log::error() << "Invalid MSZ command: map dimensions must be positive, got " << width << "x" << height << "\n";
        return;
    }
    if (_gameState._mapWidth > 0 || _gameState._mapHeight > 0) {
        Log::error() << "MSZ command received but map dimensions are already set to "
                     << _gameState._mapWidth << "x" << _gameState._mapHeight << "\n";
        return;
    }
    _gameState._mapWidth = width;
    _gameState._mapHeight = height;

    sf::Vector2u winSize = _coordinator.getResource<sf::RenderWindow>().getSize();
    sf::Vector2i origin = WorldGenerator::computeOrigin(static_cast<int>(width), static_cast<int>(height),
                                                        static_cast<int>(winSize.x), static_cast<int>(winSize.y));

    _gameState._tileOriginX = origin.x;
    _gameState._tileOriginY = origin.y;
    Log::info() << "Map dimensions set to " << width << "x" << height << " tiles\n";
}
