/*
** EPITECH PROJECT, 2026
** gui/src/Components/TileAnchor
** File description:
** TileAnchor component header file
*/

#pragma once

#include <cstdint>

/* @brief Tags an entity as standing on a given map tile, so it can be lifted
 * together with that tile (players, eggs, and anything tile-bound). */
struct TileAnchor
{
    std::uint32_t _x = 0;
    std::uint32_t _y = 0;
};
