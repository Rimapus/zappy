/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TileResourceSystem/TileResourceSystem
** File description:
** TileResourceSystem source file
*/

#include "TileResourceSystem.hpp"
#include "../../Components/Inventory.hpp"
#include "../../Components/Offsets.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Resources/TextureManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

/**
 * @brief Resource quantity to sprite sheet index
 *
 *  1 / 2 / 4 / 6 / 8+ items on the tile.
 *
 * @param qty The resource quantity on the tile
 *
 * @return The frame index to sample from the sprite sheet.
 */
static int ressourceFrame(std::uint32_t qty)
{
    if (qty >= 8) {
        return 4;
    }
    if (qty >= 6) {
        return 3;
    }
    if (qty >= 4) {
        return 2;
    }
    if (qty >= 2) {
        return 1;
    }
    return 0;
}

/**
 * @brief Draw the resource sprite frame on every tile entity.
 *
 * @param coordinator The coordinator to access components and resources
 */
void TileResourceSystem::update(Coordinator &coordinator)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    TextureManager &textures = coordinator.getResource<TextureManager>();

    for (Entity const &entity : _entities) {
        Transform &transform = coordinator.getComponent<Transform>(entity);
        Inventory &inv = coordinator.getComponent<Inventory>(entity);

        Offsets &offsets = coordinator.getComponent<Offsets>(entity);

        constexpr float tileCenter = static_cast<float>(WORLD_TILE_PX) / 2.f;

        for (int i = 0; i < 7; ++i) {
            std::uint32_t qty = inv._resources[i];
            if (qty == 0) {
                continue;
            }
            bool isFood = (i == 0);
            float scale = isFood ? FOOD_SCALE : MINERAL_SCALE;
            float half = RESSOURCES_SIZE * scale / 2.f;
            int frame = ressourceFrame(qty);
            sf::Sprite sprite(textures.get(std::string("ressource_") + RESOURCE_NAMES[i]));
            sprite.setTextureRect(sf::IntRect({frame * RESSOURCES_SIZE, 0}, {RESSOURCES_SIZE, RESSOURCES_SIZE}));
            sprite.setScale({scale, scale});
            sprite.setPosition({static_cast<float>(transform._x) + tileCenter + offsets._x[i] - half,
                                static_cast<float>(transform._y) + tileCenter + offsets._y[i] - half});
            window.draw(sprite);
        }
    }
}
