/*
** EPITECH PROJECT, 2026
** Observation.hpp
** File description:
** Aggregated game state handed to the decision model
*/

#pragma once

#include "parsing/Inventory.hpp"
#include "parsing/Look.hpp"

#include <string>
#include <vector>

namespace ai {

/**
 * @struct Broadcast
 * @brief A single broadcast message perceived by the player.
 *
 * Produced from a `message K, text` server push. The direction encodes where
 * the sound came from relative to the player's orientation.
 */
struct Broadcast
{
    int _direction{0};    ///< Sound origin tile (0 = own tile, 1-8 around it).
    std::string _message; ///< Text payload sent by the emitting player.
};

/**
 * @struct Observation
 * @brief Full snapshot of the game state for one decision step.
 *
 * Aggregates everything the model needs to choose an action: the player's
 * elevation level, its inventory, what it currently sees, the broadcasts it has
 * received, and how many team slots are still free. It is meant to be rebuilt
 * (or updated) once per decision step and fed as a single object to the model.
 */
struct Observation
{
    int _level{1};        ///< Current elevation level (1-8).
    Inventory _inventory; ///< Food and the six stones the player carries.

    /// Tiles returned by the last Look, index 0 being the player's own tile.
    std::vector<Tile> _vision;

    /// Broadcasts received since the previous step (oldest first).
    std::vector<Broadcast> _broadcasts;

    /// Free team slots, as reported by the Connect_nbr command.
    int _freeSlots{0};

    /// @brief Drop the broadcasts accumulated for this step.
    void clearBroadcasts() noexcept { _broadcasts.clear(); }
};

} // namespace ai
