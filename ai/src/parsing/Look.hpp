/*
** EPITECH PROJECT, 2026
** Look.hpp
** File description:
** Parser turning the raw Look response into an exploitable tile vector
*/

#pragma once

#include <string>
#include <vector>

namespace ai {

/**
 * @enum ObjectType
 * @brief Every kind of object a tile can contain in the Look response.
 *
 * Covers the player marker and the seven stones/resources described by the
 * subject. Any token that does not match a known name is mapped to
 * ObjectType::Unknown so parsing never fails on unexpected input.
 */
enum class ObjectType {
    Player,
    Food,
    Linemate,
    Deraumere,
    Sibur,
    Mendiane,
    Phiras,
    Thystame,
    Unknown
};

/**
 * @struct Tile
 * @brief Content of a single tile seen by the Look command.
 *
 * A tile is simply the list of objects standing on it. Duplicates are kept
 * (e.g. two players or two linemates on the same tile produce two entries), so
 * the order and multiplicity returned by the server are preserved.
 */
struct Tile
{
    std::vector<ObjectType> _objects; ///< Objects present on the tile, in order.

    /**
     * @brief Count how many objects of a given type are on the tile.
     * @param type  Object type to look for.
     * @return Number of matching objects (0 if none).
     */
    int count(ObjectType type) const;

    /**
     * @brief Test whether at least one object of @p type is on the tile.
     * @param type  Object type to look for.
     * @return true if present, false otherwise.
     */
    bool contains(ObjectType type) const;

    /**
     * @brief Test whether the tile holds no object at all.
     * @return true if the tile is empty.
     */
    bool empty() const noexcept { return _objects.empty(); }
};

/**
 * @brief Convert an object name to its ObjectType.
 * @param name  Token as written by the server (e.g. "linemate", "player").
 * @return Matching ObjectType, or ObjectType::Unknown if unrecognised.
 */
ObjectType objectTypeFromString(const std::string &name);

/**
 * @brief Convert an ObjectType back to its server name.
 * @param type  Object type.
 * @return The lowercase name (e.g. "linemate"); "unknown" for Unknown.
 */
std::string objectTypeToString(ObjectType type);

/**
 * @brief Parse a raw Look response into a vector of tiles.
 *
 * The Look command answers with a bracketed, comma-separated list of tiles,
 * each tile being a space-separated list of objects, for example:
 * @code
 * [ player, food linemate, , deraumere food, ... ]
 * @endcode
 *
 * Tiles are returned in the exact order given by the server: index 0 is the
 * player's own tile, then the tiles fanning out in front of the player. Empty
 * tiles (nothing between two commas) yield an empty Tile.
 *
 * Parsing is lenient: surrounding brackets and whitespace are optional, and
 * unknown tokens are stored as ObjectType::Unknown rather than discarded.
 *
 * @param response  Raw line returned by ServerConnection::readResponse().
 * @return Vector of tiles; never empty for a well-formed response.
 */
std::vector<Tile> parseLook(const std::string &response);

} // namespace ai
