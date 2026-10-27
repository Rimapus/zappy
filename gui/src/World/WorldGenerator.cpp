/*
** EPITECH PROJECT, 2026
** gui/src/World/WorldGenerator
** File description:
** WorldGenerator source file
*/

#include "WorldGenerator.hpp"
#include "../Components/Animator.hpp"
#include "../Components/Background.hpp"
#include "../Components/Sprite.hpp"
#include "../Components/TileMap.hpp"
#include "../Components/Transform.hpp"
#include "../Constants.hpp"
#include "../ECS/Coordinator/Coordinator.hpp"
#include "../Game/GameState.hpp"
#include "../Resources/TextureManager.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <algorithm>
#include <numeric>

constexpr int TILE = 16;
constexpr int WALL_H = 3;
constexpr int PATH_WIDTH = 3;
constexpr int COAST = 1;

constexpr float LAND_DENSITY = 0.11f;
constexpr float WATER_DENSITY = 0.06f;
constexpr float EDGE_VEG_DENSITY = 0.20f;
constexpr float STONE_DENSITY = 0.08f;
constexpr float ROCK_DENSITY = 0.015f;
constexpr int SHORE_RING = 2;

/* @brief A drawable decoration */
struct DecoKind
{
    const char *_textureKey;
    std::vector<sf::IntRect> _frames;
    float _frameDuration;
    int _weight;
};

/**
 * @brief Build the sub-rect of a 16px cell in a sheet.
 *
 * @param col      The cell column (0-based)
 * @param row      The cell row (0-based)
 *
 * @return The pixel sub-rect of that cell.
 */
static sf::IntRect cellRect(int col, int row)
{
    return sf::IntRect({col * WORLD_TILE_PX, row * WORLD_TILE_PX}, {WORLD_TILE_PX, WORLD_TILE_PX});
}

/**
 * @brief Build the frame list of an animation laid out on one sheet row.
 *
 * @param startCol     The column of the first frame
 * @param row          The row the frames live on
 * @param count        The number of consecutive frames
 *
 * @return The ordered list of frame sub-rects.
 */
static std::vector<sf::IntRect> framesRow(int startCol, int row, int count)
{
    std::vector<sf::IntRect> frames;

    for (int i = 0; i < count; ++i) {
        frames.push_back(cellRect(startCol + i, row));
    }
    return frames;
}

/**
 * @brief The weighted catalog of grass decorations.
 *
 * @return The catalog of decorations that may grow on the grass.
 */
static const std::vector<DecoKind> &landCatalog()
{
    static const std::vector<DecoKind> catalog = {
        {"grass", framesRow(0, 3, 4), 0.25f, 5},
        {"grass", framesRow(4, 3, 4), 0.25f, 3},
        {"grass", framesRow(0, 4, 4), 0.25f, 5},
        {"grass", framesRow(4, 4, 4), 0.25f, 3},
        {"grass", framesRow(0, 5, 4), 0.25f, 4},
        {"flower", {cellRect(0, 0)}, 0.f, 2},
        {"flower", {cellRect(0, 3)}, 0.f, 2},
        {"flower", {cellRect(0, 4)}, 0.f, 1},
        {"flower", {cellRect(0, 5)}, 0.f, 2},
        {"flower", {cellRect(2, 1)}, 0.f, 1},
    };

    return catalog;
}

/**
 * @brief The weighted catalog of water decorations.
 *
 * @return The catalog of decorations that may float on the water.
 */
static const std::vector<DecoKind> &waterCatalog()
{
    static const std::vector<DecoKind> catalog = {
        {"lily", framesRow(0, 0, 4), 0.35f, 2},
        {"lily", framesRow(0, 1, 4), 0.35f, 2},
        {"lily", framesRow(0, 2, 4), 0.35f, 1},
    };

    return catalog;
}

/**
 * @brief The weighted catalog of coastline vegetation.
 *
 * @return The catalog of bushes and tall grass overhanging the shore.
 */
static const std::vector<DecoKind> &edgeCatalog()
{
    static const std::vector<DecoKind> catalog = {
        {"bush", {cellRect(0, 1)}, 0.f, 3},
        {"bush", {cellRect(1, 1)}, 0.f, 3},
        {"bush", {cellRect(4, 1)}, 0.f, 2},
        {"bush", {cellRect(5, 1)}, 0.f, 2},
        {"grass", framesRow(4, 3, 4), 0.25f, 3},
        {"grass", framesRow(4, 4, 4), 0.25f, 3},
    };

    return catalog;
}

/**
 * @brief The weighted catalog of shore pebbles.
 *
 * @return The catalog of pebble clusters poking out of the shallow water.
 */
static const std::vector<DecoKind> &stoneCatalog()
{
    static const std::vector<DecoKind> catalog = {
        {"stone", {cellRect(0, 0)}, 0.f, 1},
        {"stone", {cellRect(1, 0)}, 0.f, 1},
        {"stone", {cellRect(2, 0)}, 0.f, 1},
        {"stone", {cellRect(3, 0)}, 0.f, 1},
    };

    return catalog;
}

/**
 * @brief The weighted catalog of shore rocks.
 *
 * @return The catalog of big 32x32 rocks straddling the waterline.
 */
static const std::vector<DecoKind> &rockCatalog()
{
    static const std::vector<DecoKind> catalog = {
        {"rock", {sf::IntRect({0, 0}, {2 * WORLD_TILE_PX, 2 * WORLD_TILE_PX})},                 0.f, 1},
        {"rock", {sf::IntRect({2 * WORLD_TILE_PX, 0}, {2 * WORLD_TILE_PX, 2 * WORLD_TILE_PX})}, 0.f, 1},
        {"rock", {sf::IntRect({4 * WORLD_TILE_PX, 0}, {2 * WORLD_TILE_PX, 2 * WORLD_TILE_PX})}, 0.f, 1},
        {"rock", {sf::IntRect({6 * WORLD_TILE_PX, 0}, {2 * WORLD_TILE_PX, 2 * WORLD_TILE_PX})}, 0.f, 1},
    };

    return catalog;
}

/**
 * @brief Pick a decoration from a catalog, biased by each entry's weight.
 *
 * @param catalog      The catalog to pick from
 * @param rng          The random engine
 *
 * @return A reference to the chosen decoration.
 */
static const DecoKind &weightedPick(const std::vector<DecoKind> &catalog, std::mt19937 &rng)
{
    int total = std::accumulate(catalog.begin(), catalog.end(), 0, [](int sum, const DecoKind &kind) { return sum + kind._weight; });
    std::uniform_int_distribution<int> dist(0, total - 1);
    int roll = dist(rng);

    for (const DecoKind &kind : catalog) {
        if (roll < kind._weight) {
            return kind;
        }
        roll -= kind._weight;
    }
    return catalog.back();
}

/**
 * @brief Spawn one decoration entity at a pixel position.
 *
 * @param coordinator      The coordinator to spawn the entity into
 * @param textures         The texture manager owning the sheets
 * @param kind             The decoration to spawn
 * @param px               The x pixel position
 * @param py               The y pixel position
 * @param rng              The random engine, for the animation phase
 */
static void spawnDeco(Coordinator &coordinator, TextureManager &textures, const DecoKind &kind, int px, int py,
                      std::mt19937 &rng)
{
    Entity entity = coordinator.createEntity();
    sf::Sprite sprite(textures.get(kind._textureKey));

    sprite.setTextureRect(kind._frames.front());
    coordinator.addComponent<Transform>(entity, Transform{px, py, 0});
    coordinator.addComponent<Sprite>(entity, Sprite{sprite});
    if (kind._frames.size() <= 1) {
        return;
    }
    std::uniform_int_distribution<int> phase(0, static_cast<int>(kind._frames.size()) - 1);
    Animator animator;

    animator._clips.emplace("anim", AnimationClip{kind._frames, kind._frameDuration, true});
    animator._current = "anim";
    animator._frameIndex = phase(rng);
    animator._playing = true;
    coordinator.addComponent<Animator>(entity, animator);
}

/**
 * @brief Build a generator bound to a seed and a map size.
 *
 * @param seed       The seed feeding the random engine
 * @param mapWidth   The playable grid width in tiles
 * @param mapHeight  The playable grid height in tiles
 */
WorldGenerator::WorldGenerator(unsigned int seed, int mapWidth, int mapHeight, int winWidth, int winHeight)
    : _rng(seed), _mapW(std::max(1, mapWidth)), _mapH(std::max(1, mapHeight)), _winW(std::max(1, winWidth)), _winH(std::max(1, winHeight))
{
    sf::Vector2i origin = computeOrigin(_mapW, _mapH, _winW, _winH);

    setOrigin(origin.x, origin.y);
}

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
sf::Vector2i WorldGenerator::computeOrigin(int tilesWide, int tilesHigh, int winWidth, int winHeight)
{
    int gridW = std::max(1, tilesWide);
    int gridH = std::max(1, tilesHigh);
    int visualPxW = (gridW + 2 * COAST) * TILE;
    int visualPxH = (gridH + COAST + WALL_H) * TILE;
    int visualX = std::max(0, (std::max(1, winWidth) - visualPxW) / 2);
    int visualY = std::max(0, (std::max(1, winHeight) - visualPxH) / 2);

    return {visualX + COAST * TILE, visualY + COAST * TILE};
}

/**
 * @brief Set the playable origin and recompute the cached island cell rect.
 *
 * @param originX  The pixel x of playable tile (0,0)
 * @param originY  The pixel y of playable tile (0,0)
 */
void WorldGenerator::setOrigin(int originX, int originY)
{
    _originX = originX;
    _originY = originY;
    _islandX0 = (_originX - COAST * TILE) / TILE;
    _islandX1 = (_originX + (_mapW + COAST) * TILE) / TILE - 1;
    _islandY0 = (_originY - COAST * TILE) / TILE;
    _islandY1 = (_originY + (_mapH + WALL_H) * TILE) / TILE - 1;
}

/**
 * @brief The pixel bounds of the generated island (centered in the window).
 *
 * @return The island rectangle in world pixels (position + size).
 */
sf::FloatRect WorldGenerator::islandBounds() const
{
    float width = static_cast<float>((_mapW + 2 * COAST) * TILE);
    float height = static_cast<float>((_mapH + COAST + WALL_H) * TILE);
    float left = static_cast<float>(_originX - COAST * TILE);
    float top = static_cast<float>(_originY - COAST * TILE);

    return sf::FloatRect({left, top}, {width, height});
}

/**
 * @brief The pixel center of the generated island.
 *
 * @return The island center in world pixels.
 */
sf::Vector2f WorldGenerator::islandCenter() const
{
    sf::FloatRect bounds = islandBounds();

    return {bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f};
}

/**
 * @brief Chebyshev distance, in cells, from a window cell to the island rect.
 *
 * @param x        The cell column
 * @param y        The cell row
 *
 * @return 0 when the cell is on the island, otherwise the ring index around it.
 */
int WorldGenerator::islandDistance(int x, int y) const
{
    int dx = (x < _islandX0) ? _islandX0 - x : (x > _islandX1 ? x - _islandX1 : 0);
    int dy = (y < _islandY0) ? _islandY0 - y : (y > _islandY1 ? y - _islandY1 : 0);

    return std::max(dx, dy);
}

/**
 * @brief Spawn the full world into the coordinator (call once).
 *
 * @param coordinator      The coordinator to spawn entities and read resources from
 */
void WorldGenerator::generate(Coordinator &coordinator)
{
    GameState &gameState = coordinator.getResource<GameState>();

    setOrigin(gameState._tileOriginX, gameState._tileOriginY);

    std::vector<TileType> path = buildPathGrid();

    loadAssets(coordinator);
    createBackground(coordinator);
    buildTerrain(coordinator, path);
    scatterLand(coordinator, path);
    scatterShore(coordinator, path);
    scatterWater(coordinator);
}

/**
 * @brief Load the world textures and define the tile blobs.
 *
 * @param coordinator      The coordinator holding the TextureManager / TileSet resources
 */
void WorldGenerator::loadAssets(Coordinator &coordinator)
{
    TextureManager &textures = coordinator.getResource<TextureManager>();
    TileSet &tileset = coordinator.getResource<TileSet>();

    textures.load("summer", "assets/Tileset/Summer/summer.png");
    textures.load("dirt_path", "assets/Tileset/Dirt/dirt_path.png");
    textures.load("wall", "assets/Tileset/Dirt/dirt_wall.png");
    textures.load("water", "assets/Tileset/Water/Summer-Winter/Deep/water-summer-deep1.png");
    textures.load("grass", "assets/Props/Grass.png");
    textures.load("flower", "assets/Props/Flower.png");
    textures.load("lily", "assets/Props/Water_Lily/water_lily_1.png");
    textures.load("bush", "assets/Props/Bush.png");
    textures.load("stone", "assets/Props/Stone/stone.png");
    textures.load("rock", "assets/Props/Rock/rock.png");

    tileset.defineBlob(TileType::Grass, "summer", 0, 0, WORLD_TILE_PX);
    tileset.defineBlob(TileType::Path, "dirt_path", 0, 0, WORLD_TILE_PX);
    tileset.defineBlob(TileType::Wall, "wall", 0, 0, WORLD_TILE_PX);
}

/**
 * @brief Build the overlay grid of Path / Empty cells.
 *
 * @return The width * height path overlay grid.
 */
std::vector<TileType> WorldGenerator::buildPathGrid()
{
    std::vector<TileType> path(static_cast<std::size_t>(_mapW) * _mapH, TileType::Empty);
    int minSpan = 2 * 4 + PATH_WIDTH;

    if (_mapW < minSpan || _mapH < minSpan) {
        return path;
    }
    auto paintH = [&](int row, int x0, int x1) {
        for (int y = row; y < row + PATH_WIDTH; ++y) {
            for (int x = x0; x < x1; ++x) {
                path[y * _mapW + x] = TileType::Path;
            }
        }
    };
    auto paintV = [&](int col, int y0, int y1) {
        for (int x = col; x < col + PATH_WIDTH; ++x) {
            for (int y = y0; y < y1; ++y) {
                path[y * _mapW + x] = TileType::Path;
            }
        }
    };
    int horizontals = range(1, 2);
    int verticals = range(1, 2);

    for (int i = 0; i < horizontals; ++i) {
        paintH(range(4, _mapH - 4 - PATH_WIDTH), 2, _mapW - 2);
    }
    for (int i = 0; i < verticals; ++i) {
        paintV(range(4, _mapW - 4 - PATH_WIDTH), 2, _mapH - 2);
    }
    return path;
}

/**
 * @brief Spawn the grass island, the path overlay and the dirt foundation.
 *
 * @param coordinator      The coordinator to spawn the tilemap entities into
 * @param path             The path overlay grid from buildPathGrid()
 */
void WorldGenerator::buildTerrain(Coordinator &coordinator, const std::vector<TileType> &path)
{
    int marginPx = COAST * TILE;
    int grassW = _mapW + 2 * COAST;
    int grassH = _mapH + COAST + 1;
    Entity grass = coordinator.createEntity();
    TileMap grassMap;

    grassMap._width = static_cast<std::uint32_t>(grassW);
    grassMap._height = static_cast<std::uint32_t>(grassH);
    grassMap._tileSize = TILE;
    grassMap._tiles.assign(static_cast<std::size_t>(grassW) * grassH, TileType::Grass);
    coordinator.addComponent<Transform>(grass, Transform{_originX - marginPx, _originY - marginPx, 0});
    coordinator.addComponent<TileMap>(grass, grassMap);

    Entity trail = coordinator.createEntity();
    TileMap trailMap;

    trailMap._width = _mapW;
    trailMap._height = _mapH;
    trailMap._tileSize = TILE;
    trailMap._tiles = path;
    coordinator.addComponent<Transform>(trail, Transform{_originX, _originY, 0});
    coordinator.addComponent<TileMap>(trail, trailMap);

    Entity foundation = coordinator.createEntity();
    TileMap wallMap;

    wallMap._width = static_cast<std::uint32_t>(grassW);
    wallMap._height = WALL_H;
    wallMap._tileSize = TILE;
    wallMap._tiles.assign(static_cast<std::size_t>(grassW) * WALL_H, TileType::Wall);
    coordinator.addComponent<Transform>(foundation, Transform{_originX - marginPx, _originY + _mapH * TILE, 0});
    coordinator.addComponent<TileMap>(foundation, wallMap);
}

/**
 * @brief Scatter grass tufts and flowers over the grass, off the paths.
 *
 * @param coordinator      The coordinator to spawn the decoration entities into
 * @param path             The path overlay grid, used to keep props off the trail
 */
void WorldGenerator::scatterLand(Coordinator &coordinator, const std::vector<TileType> &path)
{
    TextureManager &textures = coordinator.getResource<TextureManager>();

    for (int y = 2; y < _mapH - 2; ++y) {
        for (int x = 2; x < _mapW - 2; ++x) {
            if (path[y * _mapW + x] == TileType::Path || chance(LAND_DENSITY) == false) {
                continue;
            }
            int px = _originX + x * TILE + range(-2, 2);
            int py = _originY + y * TILE + range(-2, 2);

            spawnDeco(coordinator, textures, weightedPick(landCatalog(), _rng), px, py, _rng);
        }
    }
}

/**
 * @brief Dress the coastline with vegetation, pebbles and straddling rocks.
 *
 * @param coordinator      The coordinator to spawn the decoration entities into
 * @param path             The path overlay grid, used to keep props off the trail
 */
void WorldGenerator::scatterShore(Coordinator &coordinator, const std::vector<TileType> &path)
{
    TextureManager &textures = coordinator.getResource<TextureManager>();

    for (int y = 1; y < _mapH - 2; ++y) {
        for (int x = 1; x < _mapW - 1; ++x) {
            bool border = x < 3 || x >= _mapW - 3 || y < 3;

            if (border == false || path[y * _mapW + x] == TileType::Path ||
                chance(EDGE_VEG_DENSITY) == false) {
                continue;
            }
            int ox = (x < 3) ? -4 : (x >= _mapW - 3 ? 4 : 0);
            int oy = (y < 3) ? -4 : 0;
            int px = _originX + x * TILE + ox + range(-2, 2);
            int py = _originY + y * TILE + oy + range(-2, 2);

            spawnDeco(coordinator, textures, weightedPick(edgeCatalog(), _rng), px, py, _rng);
        }
    }

    for (int y = 0; y < _winH / TILE; ++y) {
        for (int x = 0; x < _winW / TILE; ++x) {
            int distance = islandDistance(x, y);

            if (distance < 1 || distance > SHORE_RING) {
                continue;
            }
            if (chance(STONE_DENSITY) == true) {
                spawnDeco(coordinator, textures, weightedPick(stoneCatalog(), _rng), x * TILE, y * TILE,
                          _rng);
            } else if (chance(ROCK_DENSITY) == true) {
                int rx = (x < _islandX0) ? 6 : (x > _islandX1 ? -6 : 0);
                int ry = (y < _islandY0) ? 6 : (y > _islandY1 ? -6 : 0);

                spawnDeco(coordinator, textures, weightedPick(rockCatalog(), _rng),
                          x * TILE + rx - TILE / 2, y * TILE + ry - TILE / 2, _rng);
            }
        }
    }
}

/**
 * @brief Scatter lily pads across the open water around the island.
 *
 * @param coordinator      The coordinator to spawn the decoration entities into
 */
void WorldGenerator::scatterWater(Coordinator &coordinator)
{
    TextureManager &textures = coordinator.getResource<TextureManager>();

    for (int y = 0; y < _winH / TILE; ++y) {
        for (int x = 0; x < _winW / TILE; ++x) {
            if (islandDistance(x, y) <= SHORE_RING) {
                continue;
            }
            if (chance(WATER_DENSITY) == true) {
                spawnDeco(coordinator, textures, weightedPick(waterCatalog(), _rng), x * TILE, y * TILE,
                          _rng);
            } else if (chance(ROCK_DENSITY) == true) {
                spawnDeco(coordinator, textures, weightedPick(rockCatalog(), _rng),
                          x * TILE - TILE / 2, y * TILE - TILE / 2, _rng);
            }
        }
    }
}

/**
 * @brief Spawn the tiled water background entity.
 *
 * @param coordinator      The coordinator to spawn the background entity into
 */
void WorldGenerator::createBackground(Coordinator &coordinator)
{
    Entity background = coordinator.createEntity();

    coordinator.addComponent<Background>(background, Background{
                                                         "water", sf::IntRect{{WORLD_TILE_PX, WORLD_TILE_PX},
                                                                              {WORLD_TILE_PX, WORLD_TILE_PX}}
    });
}

/**
 * @brief Draw a uniform integer in [lo, hi].
 *
 * @param lo       The inclusive lower bound
 * @param hi       The inclusive upper bound
 *
 * @return The drawn value.
 */
int WorldGenerator::range(int lo, int hi)
{
    std::uniform_int_distribution<int> dist(lo, hi);

    return dist(_rng);
}

/**
 * @brief Roll a success with the given probability.
 *
 * @param probability      The success probability in [0, 1]
 *
 * @return true with the given probability.
 */
bool WorldGenerator::chance(float probability)
{
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    return dist(_rng) < probability;
}
