/*
** EPITECH PROJECT, 2026
** gui/src/Tiles/TileSet
** File description:
** TileSet resource header file
*/

#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>

/* @brief Logical tile identifier, decoupled from any asset on disk */
enum class TileType : std::uint16_t {
    Empty = 0,
    Grass = 1,
    Wall = 2,
    Path = 3,
};

/* @brief How a TileType is drawn: a sub-rect inside a TextureManager texture */
struct TileDefinition
{
    std::string _textureKey;
    sf::IntRect _rect;
    int _blobCol = -1;
    int _blobRow = -1;
    int _cellSize = 16;
};

/* @brief TileSet Class */
class TileSet
{
public:
    /**
     * @brief Bind a tile type to a 3x3 autotile blob inside a sheet.
     *
     * @param type        The logical tile type to define
     * @param textureKey  The TextureManager key of the sheet to sample from
     * @param blobCol     The top-left column of the blob (0-based)
     * @param blobRow     The top-left row of the blob (0-based)
     * @param cellSize    The size of a cell in pixels
     */
    void defineBlob(TileType type, const std::string &textureKey, int blobCol, int blobRow, int cellSize);

    /**
     * @brief Pick the blob sub-rect to draw, given which neighbours are the same type.
     *
     * @param type        The logical tile type to draw
     * @param sameLeft    Whether the left neighbour is the same type
     * @param sameRight   Whether the right neighbour is the same type
     * @param sameTop     Whether the top neighbour is the same type
     * @param sameBottom  Whether the bottom neighbour is the same type
     *
     * @return The sub-rect to sample from the texture.
     */
    sf::IntRect rectFor(TileType type, bool sameLeft, bool sameRight, bool sameTop, bool sameBottom) const;

    /**
     * @brief Get the definition bound to a tile type.
     *
     * @param type        The logical tile type to look up
     *
     * @return A reference to the tile definition.
     */
    const TileDefinition &get(TileType type) const;

    /**
     * @brief Tell whether a tile type has a definition.
     *
     * @param type        The logical tile type to look up
     *
     * @return true if the type is defined, false otherwise.
     */
    bool has(TileType type) const;

private:
    std::unordered_map<TileType, TileDefinition> _definitions;
};
