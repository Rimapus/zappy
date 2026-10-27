/*
** EPITECH PROJECT, 2026
** gui/src/Components/TileMap
** File description:
** TileMap component header file
*/

#pragma once

#include "../Tiles/TileSet.hpp"
#include <cstdint>
#include <vector>

/* @brief TileMap Component (the whole ground grid, held by one entity) */
struct TileMap
{
    std::uint32_t _width = 0;
    std::uint32_t _height = 0;
    std::uint32_t _tileSize = 16;
    std::vector<TileType> _tiles;
};
