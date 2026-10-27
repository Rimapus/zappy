/*
** EPITECH PROJECT, 2026
** gui/src/Systems/BackgroundSystem/BackgroundSystem
** File description:
** BackgroundSystem source file
*/

#include "BackgroundSystem.hpp"
#include "../../Components/Background.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Resources/TextureManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

/**
 * @brief Tile every background over the whole window.
 *
 * @param coordinator      The coordinator to access components and resources
 */
void BackgroundSystem::update(Coordinator &coordinator)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    TextureManager &textures = coordinator.getResource<TextureManager>();

    for (Entity const &entity : _entities) {
        Background &background = coordinator.getComponent<Background>(entity);
        sf::Sprite sprite(textures.get(background._textureKey));
        sf::Vector2u size = window.getSize();

        sprite.setTextureRect(background._rect);
        for (int y = 0; y < static_cast<int>(size.y); y += background._rect.size.y) {
            for (int x = 0; x < static_cast<int>(size.x); x += background._rect.size.x) {
                sprite.setPosition({static_cast<float>(x), static_cast<float>(y)});
                window.draw(sprite);
            }
        }
    }
}
