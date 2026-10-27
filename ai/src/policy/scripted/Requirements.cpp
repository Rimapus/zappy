/*
** EPITECH PROJECT, 2026
** Requirements.cpp
** File description:
** Elevation requirements table and inventory-deficit helpers
*/

#include "policy/scripted/Requirements.hpp"

#include "parsing/Inventory.hpp"

namespace ai {

namespace {

/// Requirements indexed by [level - 1], for level in 1..7 (climb to level+1).
constexpr std::array<LevelRequirement, 7> REQUIREMENTS{
    {
     {1, {1, 0, 0, 0, 0, 0}}, // 1 -> 2
        {2, {1, 1, 1, 0, 0, 0}}, // 2 -> 3
        {2, {2, 0, 1, 0, 0, 0}}, // 3 -> 4
        {4, {1, 1, 2, 0, 1, 0}}, // 4 -> 5
        {4, {1, 2, 1, 3, 0, 0}}, // 5 -> 6
        {6, {1, 2, 3, 0, 1, 0}}, // 6 -> 7
        {6, {2, 2, 2, 2, 2, 1}}, // 7 -> 8
    }
};

/// Fallback used for out-of-range levels (e.g. a maxed-out drone).
constexpr LevelRequirement NO_REQUIREMENT{
    1, {0, 0, 0, 0, 0, 0}
};

} // namespace

const LevelRequirement &requirementFor(int level)
{
    if (level < 1 || level > 7) {
        return NO_REQUIREMENT;
    }
    return REQUIREMENTS[static_cast<std::size_t>(level - 1)];
}

std::vector<StoneDeficit> missingStones(int level, const Inventory &inventory)
{
    const LevelRequirement &requirement = requirementFor(level);
    std::vector<StoneDeficit> deficits;

    for (std::size_t i = 0; i < STONE_ORDER.size(); i++) {
        const int need = requirement._stones[i];
        const int have = inventory.get(STONE_ORDER[i]);
        if (have < need) {
            deficits.push_back({STONE_ORDER[i], need - have});
        }
    }
    return deficits;
}

} // namespace ai
