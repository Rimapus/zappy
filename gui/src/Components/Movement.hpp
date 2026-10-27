/*
** EPITECH PROJECT, 2026
** gui/src/Components/Movement
** File description:
** Movement component header file
*/

#pragma once

#include <cstdint>

/* @brief Holds the state of a tile-to-tile movement interpolation. */
struct Movement
{
    std::int32_t _startX = 0;
    std::int32_t _startY = 0;
    std::int32_t _targetX = 0;
    std::int32_t _targetY = 0;
    float _progress = 1.0f;
    float _duration = 1.0f;
};
