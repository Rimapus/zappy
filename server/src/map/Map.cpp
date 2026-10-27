/*
** EPITECH PROJECT, 2026
** Map.cpp
** File description:
** Map
*/

#include "Map.hpp"
#include "Data.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/resources/IResource.hpp"
#include "resources/Deraumere.hpp"
#include "resources/Food.hpp"
#include "resources/Linemate.hpp"
#include "resources/Mendiane.hpp"
#include "resources/Phiras.hpp"
#include "resources/Sibur.hpp"
#include "resources/Thystame.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace server {

struct ResourceDensity
{
    ResourceType _type;
    double _density;
};

static constexpr std::array<ResourceDensity, 7> RESOURCE_DENSITIES{
    {
     {ResourceType::FOOD, 0.5},
     {ResourceType::LINEMATE, 0.3},
     {ResourceType::DERAUMERE, 0.15},
     {ResourceType::SIBUR, 0.1},
     {ResourceType::MENDIANE, 0.1},
     {ResourceType::PHIRAS, 0.08},
     {ResourceType::THYSTAME, 0.05},
     }
};

static std::unique_ptr<IResource> createResource(ResourceType type)
{
    switch (type._value) {
    case ResourceType::FOOD:
        return std::make_unique<Food>();
    case ResourceType::LINEMATE:
        return std::make_unique<Linemate>();
    case ResourceType::DERAUMERE:
        return std::make_unique<Deraumere>();
    case ResourceType::SIBUR:
        return std::make_unique<Sibur>();
    case ResourceType::MENDIANE:
        return std::make_unique<Mendiane>();
    case ResourceType::PHIRAS:
        return std::make_unique<Phiras>();
    case ResourceType::THYSTAME:
        return std::make_unique<Thystame>();
    default:
        return nullptr;
    }
}

template <typename F>
static std::vector<std::pair<std::size_t, std::size_t>> buildCoordinates(const Map &map, F countFunc)
{
    std::vector<std::pair<std::size_t, std::size_t>> coordinates;
    std::size_t minCount = std::numeric_limits<std::size_t>::max();

    coordinates.reserve(map.width() * map.height());
    for (std::size_t y = 0; y < map.height(); y++) {
        for (std::size_t x = 0; x < map.width(); x++) {
            const std::size_t count = countFunc(x, y);

            if (count < minCount) {
                coordinates.clear();
                coordinates.emplace_back(x, y);
                minCount = count;
                continue;
            }
            if (count == minCount) {
                coordinates.emplace_back(x, y);
            }
        }
    }
    return coordinates;
}

static void spawnTypeOnMap(ServerData &data, Map &map, ResourceType type, std::size_t amount)
{
    if (amount == 0 || map.width() == 0 || map.height() == 0) {
        return;
    }

    static std::mt19937 rng{std::random_device{}()};
    for (std::size_t i = 0; i < amount; i++) {
        auto coordinates = buildCoordinates(map, [&map](std::size_t x, std::size_t y) {
            return map.at(x, y).resourceCount();
        });

        if (coordinates.empty()) {
            return;
        }

        std::uniform_int_distribution<std::size_t> distribution(0, coordinates.size() - 1);
        const auto [x, y] = coordinates[distribution(rng)];
        auto resource = createResource(type);
        if (resource) {
            map.at(x, y).addResource(std::move(resource));
            data.getGui().broadcast(data, &handleBct, std::format("{} {}", x, y));
        }
    }
}

std::size_t Map::countNaturalResources(ResourceType type) const
{
    std::size_t count = 0;

    for (const auto &tile : _tiles) {
        count += tile.resourceCount(type, true);
    }
    return count;
}

void Map::spawnNaturalResources(ServerData &data)
{
    const std::size_t area = _width * _height;

    if (area == 0) {
        return;
    }

    for (const auto &rule : RESOURCE_DENSITIES) {
        const std::size_t target = std::max<std::size_t>(1, static_cast<std::size_t>(static_cast<double>(area) * rule._density));
        const std::size_t current = countNaturalResources(rule._type);

        if (current < target) {
            spawnTypeOnMap(data, *this, rule._type, target - current);
        }
    }
}

std::size_t Map::spawnEggAt(std::size_t x, std::size_t y, const std::string &teamName, std::optional<std::size_t> playerId)
{
    _eggs.emplace_back(x, y, teamName, playerId);
    return _eggs.back().getId();
}

void Map::spawnEggs(const std::vector<std::string> &teams, std::size_t clientsNumber)
{
    for (const auto &team : teams) {
        for (std::size_t i = 0; i < clientsNumber; i++) {
            spawnEggForTeam(team);
        }
    }
}

void Map::spawnEggForTeam(const std::string &teamName)
{
    if (_width == 0 || _height == 0) {
        return;
    }

    static std::mt19937 rng{std::random_device{}()};
    auto coordinates = buildCoordinates(*this, [this](std::size_t x, std::size_t y) {
        return eggCountAt(x, y);
    });

    if (coordinates.empty()) {
        return;
    }

    std::uniform_int_distribution<std::size_t> distribution(0, coordinates.size() - 1);
    const auto [x, y] = coordinates[distribution(rng)];
    spawnEggAt(x, y, teamName);
}

std::size_t Map::eggCountAt(std::size_t x, std::size_t y) const
{
    std::size_t count = 0;

    for (const auto &egg : _eggs) {
        if (egg.getX() == x && egg.getY() == y) {
            count++;
        }
    }
    return count;
}

std::size_t Map::eggCountForTeam(const std::string &teamName) const
{
    std::size_t count = 0;

    for (const auto &egg : _eggs) {
        if (egg.getTeamName() == teamName) {
            count++;
        }
    }
    return count;
}

std::tuple<std::size_t, std::size_t, std::size_t> Map::consumeEggForTeam(const std::string &teamName)
{
    std::vector<std::size_t> matchingIndices;

    for (std::size_t i = 0; i < _eggs.size(); i++) {
        if (_eggs[i].getTeamName() == teamName) {
            matchingIndices.push_back(i);
        }
    }

    if (matchingIndices.empty()) {
        return {0, 0, 0};
    }

    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> distribution(0, matchingIndices.size() - 1);
    const std::size_t selectedIndex = matchingIndices[distribution(rng)];

    const auto egg = _eggs[selectedIndex];
    _eggs.erase(_eggs.begin() + selectedIndex);

    return {egg.getId(), egg.getX(), egg.getY()};
}

}
