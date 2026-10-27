/*
** EPITECH PROJECT, 2026
** VisionGeometry.cpp
** File description:
** Pure geometry helpers over the Look vision cone
*/

#include "policy/scripted/VisionGeometry.hpp"

#include <cmath>

namespace ai {

TileOffset offsetForIndex(int index)
{
    if (index <= 0) {
        return {0, 0};
    }
    const int forward = static_cast<int>(std::sqrt(static_cast<double>(index)));
    const int offsetInRow = index - forward * forward;
    return {forward, offsetInRow - forward};
}

int inferLevel(std::size_t visionSize)
{
    if (visionSize < 4) {
        return 1;
    }
    const int side = static_cast<int>(std::lround(std::sqrt(static_cast<double>(visionSize))));
    return side - 1 < 1 ? 1 : side - 1;
}

int nearestTile(const std::vector<Tile> &vision, ObjectType type)
{
    int best = -1;
    TileOffset bestOffset{};

    for (int index = 0; index < static_cast<int>(vision.size()); index++) {
        if (!vision[static_cast<std::size_t>(index)].contains(type)) {
            continue;
        }
        const TileOffset offset = offsetForIndex(index);
        if (best == -1 || offset._forward < bestOffset._forward ||
            (offset._forward == bestOffset._forward &&
             std::abs(offset._lateral) < std::abs(bestOffset._lateral))) {
            best = index;
            bestOffset = offset;
        }
    }
    return best;
}

int mostCrowdedTile(const std::vector<Tile> &vision)
{
    int best = 0;
    int bestCount = vision.empty() ? 0 : vision.front().count(ObjectType::Player);
    TileOffset bestOffset{};

    for (int index = 1; index < static_cast<int>(vision.size()); index++) {
        const int count = vision[static_cast<std::size_t>(index)].count(ObjectType::Player);
        if (count == 0) {
            continue;
        }
        const TileOffset offset = offsetForIndex(index);
        const bool closer = offset._forward < bestOffset._forward ||
                            (offset._forward == bestOffset._forward &&
                             std::abs(offset._lateral) < std::abs(bestOffset._lateral));
        if (count > bestCount || (count == bestCount && best != 0 && closer)) {
            best = index;
            bestCount = count;
            bestOffset = offset;
        }
    }
    return best;
}

std::string stepToward(int index)
{
    if (index <= 0) {
        return "";
    }
    const TileOffset offset = offsetForIndex(index);
    if (offset._lateral < 0) {
        return "Left";
    }
    if (offset._lateral > 0) {
        return "Right";
    }
    return "Forward";
}

std::string stepTowardSound(int direction)
{
    // Directions (1=ahead, rising counter-clockwise): 2/8 are the forward
    // diagonals, so walking straight already closes the gap; only the side and
    // rear bearings need a turn first (the caller forces a step after each turn).
    switch (direction) {
    case 0:
        return "";
    case 1:
    case 2:
    case 8:
        return "Forward";
    case 3:
    case 4:
    case 5:
        return "Left";
    default: // 6, 7 : to the right and behind-right
        return "Right";
    }
}

} // namespace ai
