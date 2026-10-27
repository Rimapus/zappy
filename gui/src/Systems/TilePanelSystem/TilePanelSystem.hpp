/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TilePanelSystem
** File description:
** TilePanelSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <SFML/Graphics/Rect.hpp>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

namespace sf {
class RenderWindow;
class Texture;
} // namespace sf

/* @brief System that draws the info panel of the currently selected tile */
class TilePanelSystem : public System
{
public:
    /**
     * @brief Draw the info panel of the selected tile (top-right UI overlay).
     *
     * Does nothing while no tile is selected in the GameState.
     *
     * @param coordinator      The coordinator to access resources and components
     */
    void update(Coordinator &coordinator);

private:
    /**
     * @brief Draw a 32x32 bordered texture stretched to an arbitrary rectangle
     *        using nine-slice scaling (corners kept, edges and center stretched).
     *
     * @param window      The window to draw into
     * @param texture     The 32x32 source panel texture
     * @param dest        The destination rectangle in window pixels
     */
    void drawNineSlice(sf::RenderWindow &window, const sf::Texture &texture, const sf::FloatRect &dest);
};
