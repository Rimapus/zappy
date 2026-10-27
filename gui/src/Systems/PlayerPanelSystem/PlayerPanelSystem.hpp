/*
** EPITECH PROJECT, 2026
** gui/src/Systems/PlayerPanelSystem
** File description:
** PlayerPanelSystem header file
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

/* @brief System that draws the inventory panel of the currently selected player */
class PlayerPanelSystem : public System
{
public:
    /**
     * @brief Draw the inventory panel of the selected player (top-left UI overlay).
     *
     * Does nothing while no player is selected in the GameState.
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
