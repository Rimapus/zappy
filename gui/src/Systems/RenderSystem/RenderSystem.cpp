/*
** EPITECH PROJECT, 2026
** gui/src/Systems/RenderSystem/RenderSystem
** File description:
** RenderSystem source file
*/

#include "RenderSystem.hpp"
#include "../../Components/Levitate.hpp"
#include "../../Components/Sprite.hpp"
#include "../../Components/TileAnchor.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Game/GameState.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <algorithm>
#include <utility>
#include <vector>

/**
 * @brief Look up the lift amount of the tile an entity is anchored to.
 *
 * Any entity carrying a TileAnchor (players, eggs, ...) is lifted with its
 * tile; every other entity returns 0 so its rendering is left untouched.
 *
 * @param coordinator The coordinator to access components and the game state
 * @param entity      The entity to test
 * @param tile        Receives the entity's tile coordinates when anchored
 *
 * @return The eased lift amount in [0, 1], or 0 when the entity is grounded.
 */
static float entityLift(Coordinator &coordinator, Entity entity,
                        std::pair<std::uint32_t, std::uint32_t> &tile)
{
    if (coordinator.hasComponent<TileAnchor>(entity) == false) {
        return 0.0f;
    }

    TileAnchor &anchor = coordinator.getComponent<TileAnchor>(entity);
    GameState &gameState = coordinator.getResource<GameState>();
    auto it = gameState._tileLift.find({anchor._x, anchor._y});

    tile = {anchor._x, anchor._y};
    return it == gameState._tileLift.end() ? 0.0f : it->second;
}

/**
 * @brief Blend a color toward white by the given amount.
 *
 * @param color      The base color (kept when amount is 0)
 * @param amount     The blend factor in [0, 1] (1 yields pure white)
 *
 * @return The lightened color, alpha left untouched.
 */
static sf::Color lighten(sf::Color color, float amount)
{
    auto mix = [&](std::uint8_t channel) {
        return static_cast<std::uint8_t>(channel + (255 - channel) * amount);
    };

    return sf::Color(mix(color.r), mix(color.g), mix(color.b), color.a);
}

/**
 * @brief Compute the highlight an entity gets from being the selected/hovered player.
 *
 * @param coordinator The coordinator to access the game state and player ids
 * @param entity      The entity being drawn
 * @param tint        Receives the blend-toward-white amount in [0, 1]
 *
 * @return The extra scale multiplier (1 when the entity is not highlighted).
 */
static float playerHighlight(Coordinator &coordinator, Entity entity, float &tint)
{
    GameState &gameState = coordinator.getResource<GameState>();

    tint = 0.0f;
    if (gameState._playerSelected == true) {
        auto it = gameState._players.find(gameState._selectedPlayerId);
        if (it != gameState._players.end() && it->second == entity) {
            tint = PLAYER_SELECT_TINT;
            return PLAYER_SELECT_SCALE;
        }
    }
    if (gameState._playerHovered == true) {
        auto it = gameState._players.find(gameState._hoveredPlayerId);
        if (it != gameState._players.end() && it->second == entity) {
            tint = PLAYER_HOVER_TINT;
            return PLAYER_HOVER_SCALE;
        }
    }
    return 1.0f;
}

/**
 * @brief Draw entities whose z is in [zMin, zMax] into the window resource.
 *
 * @param coordinator      The coordinator to access components and resources
 * @param zMin             Inclusive lower z bound
 * @param zMax             Inclusive upper z bound
 */
void RenderSystem::update(Coordinator &coordinator, int zMin, int zMax)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();

    std::vector<Entity> sorted(_entities.begin(), _entities.end());
    std::sort(sorted.begin(), sorted.end(), [&](Entity a, Entity b) {
        return coordinator.getComponent<Transform>(a)._z < coordinator.getComponent<Transform>(b)._z;
    });

    for (Entity const &entity : sorted) {
        Transform &transform = coordinator.getComponent<Transform>(entity);
        if (transform._z > zMax) {
            break;
        }
        if (transform._z < zMin) {
            continue;
        }
        Sprite &sprite = coordinator.getComponent<Sprite>(entity);

        if (!sprite._sprite.has_value()) {
            continue;
        }

        std::pair<std::uint32_t, std::uint32_t> tile;
        float amount = entityLift(coordinator, entity, tile);
        float posX = static_cast<float>(transform._x);
        float posY = static_cast<float>(transform._y);
        float scale = ENTITY_SCALE;

        if (amount > 0.0f) {
            GameState &gameState = coordinator.getResource<GameState>();
            float lift = 1.0f + (TILE_LIFT_SCALE - 1.0f) * amount;
            float rise = static_cast<float>(WORLD_TILE_PX) * TILE_LIFT_RISE_RATIO * amount;
            float centerX = static_cast<float>(gameState._tileOriginX + static_cast<int>(tile.first) * WORLD_TILE_PX) +
                            WORLD_TILE_PX / 2.0f;
            float centerY = static_cast<float>(gameState._tileOriginY + static_cast<int>(tile.second) * WORLD_TILE_PX) +
                            WORLD_TILE_PX / 2.0f;

            posX = centerX + (posX - centerX) * lift;
            posY = centerY + (posY - centerY) * lift - rise;
            scale = ENTITY_SCALE * lift;
        }
        if (coordinator.hasComponent<Levitate>(entity)) {
            posY -= coordinator.getComponent<Levitate>(entity)._offset;
        }
        float tint = 0.0f;
        float highlight = playerHighlight(coordinator, entity, tint);
        sf::Color original = sprite._sprite->getColor();
        float grow = static_cast<float>(PLAYER_FRAME) * scale * (highlight - 1.0f) / 2.0f;

        sprite._sprite->setPosition({posX - grow, posY - grow});
        sprite._sprite->setScale({scale * highlight, scale * highlight});
        if (tint > 0.0f) {
            sprite._sprite->setColor(lighten(original, tint));
        }
        window.draw(*sprite._sprite);
        if (tint > 0.0f) {
            sprite._sprite->setColor(original);
        }
    }
}
