/*
** EPITECH PROJECT, 2026
** Inventory.hpp
** File description:
** Parser turning the raw Inventory response into a typed structure
*/

#pragma once

#include "Look.hpp"

#include <string>

namespace ai {

/**
 * @struct Inventory
 * @brief Typed view of the player's inventory.
 *
 * Holds the food counter plus the six stones/resources of the game. Every field
 * is a quantity (number of units owned). Defaults to all-zero, which is also the
 * value used for any resource missing from a partial response.
 */
struct Inventory
{
    int _food{0};      ///< Remaining food units (each unit ≈ 126 time units).
    int _linemate{0};  ///< Number of linemate stones.
    int _deraumere{0}; ///< Number of deraumere stones.
    int _sibur{0};     ///< Number of sibur stones.
    int _mendiane{0};  ///< Number of mendiane stones.
    int _phiras{0};    ///< Number of phiras stones.
    int _thystame{0};  ///< Number of thystame stones.

    /**
     * @brief Read-only access to a quantity by object type.
     * @param type  Resource to query (Player/Unknown always yield 0).
     * @return The stored quantity for @p type.
     */
    int get(ObjectType type) const;

    /**
     * @brief Set a quantity by object type.
     *
     * No-op for ObjectType::Player and ObjectType::Unknown, so unexpected tokens
     * in a response are ignored rather than corrupting the structure.
     *
     * @param type   Resource to update.
     * @param value  New quantity.
     */
    void set(ObjectType type, int value);
};

/**
 * @brief Parse a raw Inventory response into a typed Inventory.
 *
 * The Inventory command answers with a bracketed, comma-separated list of
 * `name count` pairs, for example:
 * @code
 * [ food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0 ]
 * @endcode
 *
 * Parsing is lenient: surrounding brackets and whitespace are optional, the
 * order of pairs does not matter, missing resources keep their default of 0, and
 * unknown names are ignored.
 *
 * @param response  Raw line returned by ServerConnection::readResponse().
 * @return The corresponding Inventory.
 */
Inventory parseInventory(const std::string &response);

} // namespace ai
