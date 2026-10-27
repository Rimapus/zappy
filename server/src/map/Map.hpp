/*
** EPITECH PROJECT, 2026
** Map.hpp
** File description:
** Include file for the Map
*/

#pragma once

#include "Tile.hpp"
#include "map/Egg.hpp"
#include "map/resources/IResource.hpp"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace server {

class ServerData;

class Map
{
public:
    Map(void) = default;
    Map(const Map &) = delete;
    Map &operator=(const Map &) = delete;
    Map(Map &&) noexcept = default;
    Map &operator=(Map &&) noexcept = default;

    Map(std::size_t width, std::size_t height)
        : _width(width), _height(height), _tiles(width * height) {}

    ~Map(void) = default;

    std::size_t width(void) const
    {
        return _width;
    }

    std::size_t height(void) const
    {
        return _height;
    }

    Tile &at(std::size_t x, std::size_t y)
    {
        if (x >= _width || y >= _height) {
            throw std::out_of_range("Map::at");
        }
        return _tiles[y * _width + x];
    }

    const Tile &at(std::size_t x, std::size_t y) const
    {
        if (x >= _width || y >= _height) {
            throw std::out_of_range("Map::at");
        }
        return _tiles[y * _width + x];
    }

    std::size_t countNaturalResources(ResourceType type) const;
    void spawnNaturalResources(ServerData &data);

    auto &getEggs(void) { return _eggs; }

    auto &getEggs(void) const { return _eggs; }

    std::size_t spawnEggAt(std::size_t x, std::size_t y, const std::string &teamName, std::optional<std::size_t> playerId = std::nullopt);
    void spawnEggs(const std::vector<std::string> &teams, std::size_t clientsNumber);
    void spawnEggForTeam(const std::string &teamName);
    std::size_t eggCountAt(std::size_t x, std::size_t y) const;
    std::size_t eggCountForTeam(const std::string &teamName) const;
    std::tuple<std::size_t, std::size_t, std::size_t> consumeEggForTeam(const std::string &teamName);

private:
    std::size_t _width{0};
    std::size_t _height{0};
    std::vector<Tile> _tiles;
    std::vector<Egg> _eggs;
};

}
