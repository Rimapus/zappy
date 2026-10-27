/*
** EPITECH PROJECT, 2026
** gui/src/Systems/EndScreenSystem
** File description:
** EndScreenSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief Renders a full-window overlay on game over or server disconnection */
class EndScreenSystem : public System
{
public:
    /**
     * @brief Draw the appropriate end overlay if the game has ended or the
     *        connection was lost. No-op while the game is still running.
     *
     * @param coordinator The coordinator to access resources
     */
    void update(Coordinator &coordinator);

private:
    /**
     * @brief Dim the screen and draw the bottom hint, returning the center.
     *
     * @param window The window to draw into
     * @param font The font to render with
     *
     * @return The window center in pixels.
     */
    sf::Vector2f drawBase(sf::RenderWindow &window, const sf::Font &font) const;

    /**
     * @brief Draw the "GAME OVER — <team> wins!" overlay.
     *
     * @param window The window to draw into
     * @param font The font to render with
     * @param center The screen center returned by drawBase
     * @param winnerTeam The name of the winning team
     * @param winColor The team color
     */
    void drawGameOver(sf::RenderWindow &window, const sf::Font &font, sf::Vector2f center,
                      const std::string &winnerTeam, sf::Color winColor) const;

    /**
     * @brief Draw the "CONNECTION LOST" overlay.
     *
     * @param window The window to draw into
     * @param font The font to render with
     * @param center The screen center returned by drawBase
     */
    void drawDisconnected(sf::RenderWindow &window, const sf::Font &font, sf::Vector2f center) const;
};
