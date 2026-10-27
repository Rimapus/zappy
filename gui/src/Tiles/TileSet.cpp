/*
** EPITECH PROJECT, 2026
** gui/src/Tiles/TileSet
** File description:
** TileSet source file
*/

#include "TileSet.hpp"

/**
 * @brief Bind a tile type to a 3x3 autotile blob inside a sheet.
 *
 * @param type        The logical tile type to define
 * @param textureKey  The TextureManager key of the sheet to sample from
 * @param blobCol     The top-left column of the blob (0-based)
 * @param blobRow     The top-left row of the blob (0-based)
 * @param cellSize    The size of a cell in pixels
 */
void TileSet::defineBlob(TileType type, const std::string &textureKey, int blobCol, int blobRow, int cellSize)
{
    sf::IntRect center({(blobCol + 1) * cellSize, (blobRow + 1) * cellSize}, {cellSize, cellSize});

    _definitions[type] = TileDefinition{textureKey, center, blobCol, blobRow, cellSize};
}

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
sf::IntRect TileSet::rectFor(TileType type, bool sameLeft, bool sameRight, bool sameTop, bool sameBottom) const
{
    const TileDefinition &def = get(type);

    if (def._blobCol < 0) {
        return def._rect;
    }
    int colOffset = (sameLeft == false) ? 0 : (sameRight == false) ? 2
                                                                   : 1;
    int rowOffset = (sameTop == false) ? 0 : (sameBottom == false) ? 2
                                                                   : 1;
    int col = def._blobCol + colOffset;
    int row = def._blobRow + rowOffset;

    return sf::IntRect({col * def._cellSize, row * def._cellSize}, {def._cellSize, def._cellSize});
}

/**
 * @brief Get the definition bound to a tile type.
 *
 * @param type        The logical tile type to look up
 *
 * @return A reference to the tile definition.
 */
const TileDefinition &TileSet::get(TileType type) const
{
    return _definitions.at(type);
}

/**
 * @brief Tell whether a tile type has a definition.
 *
 * @param type        The logical tile type to look up
 *
 * @return true if the type is defined, false otherwise.
 */
bool TileSet::has(TileType type) const
{
    return _definitions.find(type) != _definitions.end();
}
