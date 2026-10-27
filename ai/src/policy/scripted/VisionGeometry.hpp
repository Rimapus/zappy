/*
** EPITECH PROJECT, 2026
** VisionGeometry.hpp
** File description:
** Pure geometry helpers over the Look vision cone - tile addressing, level
** inference and greedy navigation toward a target tile or a broadcast source.
*/

#pragma once

#include "parsing/Look.hpp"

#include <string>
#include <vector>

namespace ai {

/**
 * @struct TileOffset
 * @brief A vision tile expressed relative to the player's facing.
 *
 * @c _forward is the number of tiles straight ahead (0 = own tile). @c _lateral
 * is the sideways shift: negative to the left, positive to the right.
 */
struct TileOffset
{
    int _forward{0}; ///< Tiles ahead of the player (>= 0).
    int _lateral{0}; ///< Sideways shift (negative left, positive right).
};

/**
 * @brief Convert a Look vision index to its forward/lateral offset.
 *
 * The server lays the cone out row by row: index 0 is the own tile, then row r
 * (r tiles ahead) holds 2r+1 tiles read left to right, so index r*r is the
 * row's leftmost tile.
 *
 * @param index  Position in the Observation::_vision vector.
 * @return The matching TileOffset.
 */
TileOffset offsetForIndex(int index);

/**
 * @brief Infer the true elevation level from the size of a Look reply.
 *
 * A Look at level L returns (L+1)^2 tiles, so the level is recoverable from the
 * vector size alone - robust even when group incantations desync the level the
 * game loop tracks for non-initiators.
 *
 * @param visionSize  Number of tiles returned by the last Look.
 * @return The inferred level (>= 1), or 1 if the size is degenerate.
 */
int inferLevel(std::size_t visionSize);

/**
 * @brief Find the closest visible tile holding @p type.
 *
 * "Closest" means fewest tiles forward, ties broken by smallest sideways shift,
 * so the drone walks the shortest sensible path. The own tile (index 0) is
 * considered first.
 *
 * @param vision  Tiles from the last Look.
 * @param type    Object to home in on.
 * @return The vision index of the nearest match, or -1 if none is visible.
 */
int nearestTile(const std::vector<Tile> &vision, ObjectType type);

/**
 * @brief Most crowded visible tile, ties broken by proximity.
 *
 * Drives the final approach to a rally by climbing the player-density gradient:
 * sound gives only a coarse bearing that orbits a tile forever, but walking
 * toward whichever tile holds the most players funnels the whole team onto the
 * single densest spot (the stationary mayor seeds it). Index 0 (own tile) wins
 * ties, so a drone already on the peak is told to stay.
 *
 * @param vision  Tiles from the last Look.
 * @return Vision index of the most crowded tile (0 if that is the own tile).
 */
int mostCrowdedTile(const std::vector<Tile> &vision);

/**
 * @brief Greedy one-step command to approach the tile at @p index.
 *
 * Turns to face the target column first, then walks forward. Returns an empty
 * string when already on the target tile (index 0).
 *
 * @param index  Vision index of the destination tile.
 * @return "Forward", "Left", "Right", or "" when already there.
 */
std::string stepToward(int index);

/**
 * @brief Greedy one-step command to walk toward a broadcast source.
 *
 * @param direction  Sound tile reported by the server (0 = same tile, 1 = dead
 *                   ahead, increasing counter-clockwise through the eight
 *                   neighbours).
 * @return "Forward", "Left", "Right", or "" when the source is on this tile.
 */
std::string stepTowardSound(int direction);

} // namespace ai
