/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TileHighlightSystem
** File description:
** TileHighlightSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <vector>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

namespace sf {
class RenderWindow;
class View;
} // namespace sf

/* @brief System that raises the hovered/selected tile with a shader drop shadow */
class TileHighlightSystem : public System
{
public:
    /**
     * @brief Smoothly lift the hovered and selected tiles out of the board.
     *
     * Eases each tile's lift in and out, grabs its pixels from the
     * framebuffer and redraws them enlarged and raised over a real,
     * gaussian-blurred drop shadow, as if the tile were picked up to be
     * examined. Must be called while the window uses the world camera view.
     *
     * @param coordinator      The coordinator to access resources and components
     * @param dt               Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);

private:
    /**
     * @brief Resolve the on-screen geometry of a lifted tile.
     *
     * @param window      The window the tile is drawn into
     * @param camera      The world camera, to map the tile to screen pixels
     * @param coordinator The coordinator to access the tile transform
     * @param x           The tile x coordinate
     * @param y           The tile y coordinate
     * @param amount      The eased lift amount in [0, 1]
     * @param region      Receives the tile region in the framebuffer snapshot
     * @param topLeft     Receives the lifted sprite's top-left in screen pixels
     * @param scale       Receives the lift scale factor
     *
     * @return true if the tile exists and is fully on screen.
     */
    bool computeLift(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator, std::uint32_t x,
                     std::uint32_t y, float amount, sf::IntRect &region, sf::Vector2f &topLeft, float &scale);

    /**
     * @brief Draw a tile's shadow and raised face from the snapshot.
     *
     * @param window      The window to draw into (snapshot already taken)
     * @param camera      The world camera, to map the tile to screen pixels
     * @param coordinator The coordinator to access the tile transform
     * @param x           The tile x coordinate
     * @param y           The tile y coordinate
     * @param amount      The eased lift amount in [0, 1]
     */
    void liftTile(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator, std::uint32_t x,
                  std::uint32_t y, float amount);

    /**
     * @brief Draw a lifted tile's light rim (called after every tile face).
     *
     * @param window      The window to draw into
     * @param camera      The world camera, to map the tile to screen pixels
     * @param coordinator The coordinator to access the tile transform
     * @param x           The tile x coordinate
     * @param y           The tile y coordinate
     * @param amount      The eased lift amount in [0, 1]
     */
    void drawRim(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator, std::uint32_t x,
                 std::uint32_t y, float amount);

    /**
     * @brief Render the gaussian-blurred shadow of a tile region into _shadowA.
     *
     * @param tile      The tile region in the framebuffer snapshot
     */
    void buildShadow(const sf::IntRect &tile);

    /**
     * @brief Lazily compile the blur shader and allocate the shadow targets.
     *
     * @return true if the shader pipeline is ready, false if unsupported.
     */
    bool ensureBlur();

    /* @brief One tile easing its lift toward 1 (active) or 0 (fading out) */
    struct LiftState
    {
        std::uint32_t _x;
        std::uint32_t _y;
        float _lift;
    };

    sf::Texture _screen;
    sf::Shader _blur;
    sf::RenderTexture _shadowA;
    sf::RenderTexture _shadowB;
    bool _blurTried = false;
    bool _blurReady = false;
    std::vector<LiftState> _lifts;
};
