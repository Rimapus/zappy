/*
** EPITECH PROJECT, 2026
** gui/src/Components/Offsets
** File description:
** Offsets component header file
*/

#pragma once

#include <array>

/* @brief Stores random pixel offsets for an entity's sub-elements.
 * Players use index 0. Tiles use indices 0-6. */
struct Offsets
{
    std::array<float, 7> _x = {};
    std::array<float, 7> _y = {};
};
