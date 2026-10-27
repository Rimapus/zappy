/*
 * EPITECH PROJECT, 2026
 * PlayerOrientation.hpp
 * File description:
 * Protocol orientation → spritesheet helpers
 *
 * Protocol: 1=North  2=East  3=South  4=West
 * Sheet:    row 0=South  row 1=East  row 2=North  row 3=West
 */

#pragma once

#include <string>

/**
 * @brief Map protocol orientation to spritesheet row.
 *
 *  spritesheet : 0=South  1=East  2=North  3=West
 *  protocol O:   3=South  2=East  1=North  4=West
 *
 * @param o Protocol orientation (1-4)
 * @return int Spritesheet row index (0-3)
 */
inline int orientRow(int o)
{
    switch (o) {
    case 1:
        return 2;
    case 2:
        return 0;
    case 4:
        return 3;
    default:
        return 1;
    }
}

/**
 * @brief Map protocol orientation to name suffix.
 *
 * @param o Protocol orientation (1-4)
 * @return std::string Name suffix for that orientation
 */
inline std::string orientName(int o)
{
    switch (o) {
    case 1:
        return "north";
    case 2:
        return "east";
    case 4:
        return "west";
    default:
        return "south";
    }
}
