/*
** EPITECH PROJECT, 2026
** Tile.hpp
** File description:
** Tile
*/

#pragma once

#include "resources/IResource.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace server {

class Tile
{
public:
    Tile(void) = default;
    Tile(const Tile &) = delete;
    Tile &operator=(const Tile &) = delete;
    Tile(Tile &&) noexcept = default;
    Tile &operator=(Tile &&) noexcept = default;
    ~Tile(void) = default;

    void addResource(std::unique_ptr<IResource> resource);
    std::size_t resourceCount(void) const;
    std::size_t resourceCount(ResourceType type, bool naturalOnly) const;
    std::vector<ResourceType> getResources(void) const;
    bool hasResource(ResourceType type, std::size_t number = 1) const;
    std::unique_ptr<IResource> takeResource(ResourceType type);
    std::vector<std::unique_ptr<IResource>> takeResources(ResourceType type, std::size_t nb);

private:
    std::vector<std::unique_ptr<IResource>> _resources;
};

}
