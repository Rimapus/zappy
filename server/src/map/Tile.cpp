/*
** EPITECH PROJECT, 2026
** Tile.cpp
** File description:
** Tile
*/

#include "Tile.hpp"
#include "map/resources/IResource.hpp"
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace server {

void Tile::addResource(std::unique_ptr<IResource> resource)
{
    _resources.push_back(std::move(resource));
}

std::size_t Tile::resourceCount(void) const
{
    return _resources.size();
}

std::size_t Tile::resourceCount(ResourceType type, bool naturalOnly) const
{
    std::size_t count = 0;

    for (const auto &resource : _resources) {
        if (resource && resource->getType() == type && (!naturalOnly || resource->isNatural())) {
            count++;
        }
    }
    return count;
}

std::vector<ResourceType> Tile::getResources(void) const
{
    std::vector<ResourceType> resources;

    resources.reserve(_resources.size());
    for (const auto &resource : _resources) {
        if (resource) {
            resources.push_back(resource->getType());
        }
    }
    return resources;
}

bool Tile::hasResource(ResourceType type, std::size_t number) const
{
    if (number == 0) {
        return true;
    }

    std::size_t count = 0;

    for (const auto &resource : _resources) {
        if (resource && resource->getType() == type) {
            count++;
            if (count >= number) {
                return true;
            }
        }
    }
    return false;
}

std::unique_ptr<IResource> Tile::takeResource(ResourceType type)
{
    for (std::size_t i = 0; i < _resources.size(); i++) {
        if (_resources[i] && _resources[i]->getType() == type) {
            auto ptr = std::move(_resources[i]);
            _resources.erase(_resources.begin() + i);
            return ptr;
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<IResource>> Tile::takeResources(ResourceType type, std::size_t number)
{
    std::vector<std::unique_ptr<IResource>> taken;

    for (std::size_t i = 0; i < number; i++) {
        auto resource = takeResource(type);
        if (resource) {
            taken.push_back(std::move(resource));
        }
    }
    return taken;
}

}
