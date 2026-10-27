/*
** EPITECH PROJECT, 2026
** gui/src/World/WorldGenerator
** File description:
** WorldGenerator header file
*/

#pragma once

#include "../Tiles/TileSet.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <random>
#include <vector>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief Builds the whole world (terrain, paths, decorations) from a seed */
class WorldGenerator
{
public:
    /**
     * @brief Build a generator bound to a seed and a map size.
     *
     * @param seed       The seed feeding the random engine
     * @param mapWidth   The map width in tiles (from the server `msz`)
     * @param mapHeight  The map height in tiles (from the server `msz`)
     * @param winWidth   The window width in pixels (drives centering / ocean spread)
     * @param winHeight  The window height in pixels (drives centering / ocean spread)
     */
    WorldGenerator(unsigned int seed, int mapWidth, int mapHeight, int winWidth, int winHeight);

    /**
     * @brief Compute the centered pixel origin of the playable grid's tile (0,0).
     *
     * @param tilesWide   The playable grid width in tiles
     * @param tilesHigh   The playable grid height in tiles
     * @param winWidth    The window width in pixels
     * @param winHeight   The window height in pixels
     *
     * @return The pixel position of playable tile (0,0).
     */
    static sf::Vector2i computeOrigin(int tilesWide, int tilesHigh, int winWidth, int winHeight);

    /**
     * @brief Spawn the full world into the coordinator (call once).
     *
     * @param coordinator      The coordinator to spawn entities and read resources from
     */
    void generate(Coordinator &coordinator);

    /**
     * @brief The pixel bounds of the generated island (centered in the window).
     *
     * @return The island rectangle in world pixels (position + size).
     */
    sf::FloatRect islandBounds() const;

    /**
     * @brief The pixel center of the generated island.
     *
     * @return The island center in world pixels.
     */
    sf::Vector2f islandCenter() const;

private:
    /**
     * @brief Load the world textures and define the tile blobs.
     *
     * @param coordinator      The coordinator holding the TextureManager / TileSet resources
     */
    void loadAssets(Coordinator &coordinator);

    /**
     * @brief Build the overlay grid of Path / Empty cells.
     *
     * @return The MAP_W * GRASS_H path overlay grid.
     */
    std::vector<TileType> buildPathGrid();

    /**
     * @brief Spawn the grass island, the path overlay and the dirt foundation.
     *
     * @param coordinator      The coordinator to spawn the tilemap entities into
     * @param path             The path overlay grid from buildPathGrid()
     */
    void buildTerrain(Coordinator &coordinator, const std::vector<TileType> &path);

    /**
     * @brief Scatter grass tufts and flowers over the grass, off the paths.
     *
     * @param coordinator      The coordinator to spawn the decoration entities into
     * @param path             The path overlay grid, used to keep props off the trail
     */
    void scatterLand(Coordinator &coordinator, const std::vector<TileType> &path);

    /**
     * @brief Dress the coastline with vegetation, pebbles and straddling rocks.
     *
     * @param coordinator      The coordinator to spawn the decoration entities into
     * @param path             The path overlay grid, used to keep props off the trail
     */
    void scatterShore(Coordinator &coordinator, const std::vector<TileType> &path);

    /**
     * @brief Scatter lily pads across the open water around the island.
     *
     * @param coordinator      The coordinator to spawn the decoration entities into
     */
    void scatterWater(Coordinator &coordinator);

    /**
     * @brief Spawn the tiled water background entity.
     *
     * @param coordinator      The coordinator to spawn the background entity into
     */
    void createBackground(Coordinator &coordinator);

    /**
     * @brief Draw a uniform integer in [lo, hi].
     *
     * @param lo       The inclusive lower bound
     * @param hi       The inclusive upper bound
     *
     * @return The drawn value.
     */
    int range(int lo, int hi);

    /**
     * @brief Roll a success with the given probability.
     *
     * @param probability      The success probability in [0, 1]
     *
     * @return true with the given probability.
     */
    bool chance(float probability);

    /**
     * @brief Chebyshev distance, in cells, from a window cell to the island rect.
     *
     * @param x        The cell column
     * @param y        The cell row
     *
     * @return 0 when the cell is on the island, otherwise the ring index around it.
     */
    int islandDistance(int x, int y) const;

    /**
     * @brief Set the playable origin and recompute the cached island cell rect.
     *
     * @param originX  The pixel x of playable tile (0,0)
     * @param originY  The pixel y of playable tile (0,0)
     */
    void setOrigin(int originX, int originY);

    std::mt19937 _rng;
    int _mapW;
    int _mapH;
    int _winW;
    int _winH;
    int _originX;
    int _originY;
    int _islandX0;
    int _islandX1;
    int _islandY0;
    int _islandY1;
};
