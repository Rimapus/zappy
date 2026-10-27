/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TileMapSystem/TileMapSystem
** File description:
** TileMapSystem source file
*/

#include "TileMapSystem.hpp"
#include "../../Components/TileMap.hpp"
#include "../../Components/Transform.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Resources/TextureManager.hpp"
#include "../../Tiles/TileSet.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

/**
 * @brief Tell whether a neighbour cell holds the same tile type.
 *
 * @param map      The tilemap to read
 * @param nx       The neighbour column (may be out of bounds)
 * @param ny       The neighbour row (may be out of bounds)
 * @param type     The reference tile type to compare against
 *
 * @return true if the neighbour is in bounds and of the same type.
 */
static bool sameType(const TileMap &map, int nx, int ny, TileType type)
{
    if (nx < 0 || ny < 0 || nx >= static_cast<int>(map._width) || ny >= static_cast<int>(map._height)) {
        return false;
    }
    return map._tiles[ny * map._width + nx] == type;
}

/**
 * @brief Draw every tile of each tilemap into the window resource.
 *
 * @param coordinator      The coordinator to access components and resources
 */
void TileMapSystem::update(Coordinator &coordinator)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    TextureManager &textures = coordinator.getResource<TextureManager>();
    TileSet &tileset = coordinator.getResource<TileSet>();

    for (Entity const &entity : _entities) {
        Transform &origin = coordinator.getComponent<Transform>(entity);
        TileMap &map = coordinator.getComponent<TileMap>(entity);

        for (int y = 0; y < static_cast<int>(map._height); ++y) {
            for (int x = 0; x < static_cast<int>(map._width); ++x) {
                TileType type = map._tiles[y * map._width + x];

                if (tileset.has(type) == false) {
                    continue;
                }
                sf::IntRect rect = tileset.rectFor(type,
                                                   sameType(map, x - 1, y, type), sameType(map, x + 1, y, type),
                                                   sameType(map, x, y - 1, type), sameType(map, x, y + 1, type));
                sf::Sprite sprite(textures.get(tileset.get(type)._textureKey));
                sprite.setTextureRect(rect);
                sprite.setPosition({static_cast<float>(origin._x + x * static_cast<int>(map._tileSize)),
                                    static_cast<float>(origin._y + y * static_cast<int>(map._tileSize))});
                window.draw(sprite);
            }
        }
    }
}
