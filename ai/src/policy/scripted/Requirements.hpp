/*
** EPITECH PROJECT, 2026
** Requirements.hpp
** File description:
** Elevation requirements table and inventory-deficit helpers used by the
** scripted policy to decide what to gather and when to incant.
*/

#pragma once

#include "parsing/Inventory.hpp"
#include "parsing/Look.hpp"

#include <array>
#include <vector>

namespace ai {

/// @brief The six elevation stones, in the canonical server order.
constexpr std::array<ObjectType, 6> STONE_ORDER{
    ObjectType::Linemate, ObjectType::Deraumere, ObjectType::Sibur,
    ObjectType::Mendiane, ObjectType::Phiras, ObjectType::Thystame};

/**
 * @struct LevelRequirement
 * @brief Players and stones an incantation needs to climb one level.
 *
 * @c _stones is indexed by STONE_ORDER (linemate, deraumere, sibur, mendiane,
 * phiras, thystame).
 */
struct LevelRequirement
{
    int _players{1};              ///< Same-level players that must stand on the tile.
    std::array<int, 6> _stones{}; ///< Stone counts required on the tile.
};

/**
 * @brief Requirement to climb from @p level to @p level + 1.
 * @param level  Current elevation level (1-7); out-of-range yields a 1-player,
 *               no-stone requirement so callers never read past the table.
 * @return The matching LevelRequirement.
 */
const LevelRequirement &requirementFor(int level);

/// @brief Maximum reachable level - no incantation exists at or above it.
constexpr int MAX_LEVEL = 8;

/**
 * @struct StoneDeficit
 * @brief A stone the drone still needs and how many units are missing.
 */
struct StoneDeficit
{
    ObjectType _type{ObjectType::Unknown}; ///< Which stone is missing.
    int _missing{0};                       ///< Units still to gather.
};

/**
 * @brief List the stones the drone lacks to incant at @p level.
 * @param level      Current level (drives the requirement looked up).
 * @param inventory  The drone's current inventory.
 * @return One entry per still-missing stone (empty if the drone is ready).
 */
std::vector<StoneDeficit> missingStones(int level, const Inventory &inventory);

} // namespace ai
