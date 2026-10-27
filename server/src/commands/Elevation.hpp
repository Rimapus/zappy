/*
** EPITECH PROJECT, 2026
** Elevation.hpp
** File description:
** Elevation
*/

#pragma once

#include "map/resources/IResource.hpp"
#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace server {

class ElevationRequirement
{
public:
    std::size_t _players{};
    std::vector<std::pair<ResourceType::Value, std::size_t>> _resources{};
};

static const std::array<ElevationRequirement, 7> ELEVATION_REQUIREMENTS{
    {
     {1, {{{ResourceType::LINEMATE, 1}}}},
     {2, {{{ResourceType::LINEMATE, 1}, {ResourceType::DERAUMERE, 1}, {ResourceType::SIBUR, 1}}}},
     {2, {{{ResourceType::LINEMATE, 2}, {ResourceType::SIBUR, 1}}}},
     {4, {{{ResourceType::LINEMATE, 1}, {ResourceType::DERAUMERE, 1}, {ResourceType::SIBUR, 2}, {ResourceType::PHIRAS, 1}}}},
     {4, {{{ResourceType::LINEMATE, 1}, {ResourceType::DERAUMERE, 2}, {ResourceType::SIBUR, 1}, {ResourceType::MENDIANE, 3}}}},
     {6, {{{ResourceType::LINEMATE, 1}, {ResourceType::DERAUMERE, 2}, {ResourceType::SIBUR, 3}, {ResourceType::PHIRAS, 1}}}},
     {6, {{{ResourceType::LINEMATE, 2}, {ResourceType::DERAUMERE, 2}, {ResourceType::SIBUR, 2}, {ResourceType::MENDIANE, 2}, {ResourceType::PHIRAS, 2}, {ResourceType::THYSTAME, 1}}}},
     }
};

}
