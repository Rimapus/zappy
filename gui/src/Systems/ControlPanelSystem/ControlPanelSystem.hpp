/*
** EPITECH PROJECT, 2026
** gui/src/Systems/ControlPanelSystem
** File description:
** ControlPanelSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

namespace sf {
class RenderWindow;
class Texture;
} // namespace sf

/* @brief One clickable stepper button with its hit rectangle and step (-- - + ++). */
struct FreqButton
{
    sf::FloatRect _rect;
    int _delta = 0;
    const char *_label = "";
};

/* @brief Pre-computed geometry of the open panel, shared between drawing and clicking. */
struct PanelLayout
{
    float _freqValueY = 0.0f;
    std::array<FreqButton, 4> _freqButtons;
    float _volumeValueY = 0.0f;
    std::array<FreqButton, 4> _volumeButtons;
    sf::FloatRect _refreshRect;
};

/*
 * @brief System that draws the server-parameter panel (top-left UI overlay).
 *
 * It exposes the only game parameter the GUI is allowed to change through the
 * protocol: the time unit / frequency, tweaked with the sst command.
 */
class ControlPanelSystem : public System
{
public:
    /**
     * @brief Draw the "Settings" toggle button (bottom-right) and, while the
     *        panel is open, the frequency controls above it.
     *
     * @param coordinator      The coordinator to access resources
     */
    void update(Coordinator &coordinator);

    /**
     * @brief Route a left click (in UI coordinates) to the panel.
     *
     * Clicking the toggle button opens/closes the panel. While open, clicking a
     * frequency button sends the new frequency to the server with sst and applies
     * it optimistically; clicking anywhere else closes the panel.
     *
     * @param coordinator      The coordinator to access resources
     * @param point            The click position in UI coordinates
     *
     * @return true if the click was consumed by the panel or its toggle button.
     */
    bool handleClick(Coordinator &coordinator, sf::Vector2f point);

private:
    /**
     * @brief Compute the "Settings" toggle button rectangle (bottom-right).
     *
     * @param window      The window the button is drawn into
     *
     * @return The toggle button rectangle in window pixels.
     */
    sf::FloatRect toggleRect(const sf::RenderWindow &window) const;

    /**
     * @brief Compute the panel rectangle, anchored just above the toggle button.
     *
     * @param window      The window the panel is drawn into
     * @param teamCount   The number of teams listed in the info block
     *
     * @return The panel rectangle in window pixels.
     */
    sf::FloatRect panelRect(const sf::RenderWindow &window, std::size_t teamCount) const;

    /**
     * @brief Compute the geometry of every control inside the panel (frequency
     *        stepper, volume stepper and the force-refresh button).
     *
     * @param panel       The panel rectangle in window pixels
     * @param teamCount   The number of teams listed in the info block
     *
     * @return The pre-computed layout shared by drawing and click handling.
     */
    PanelLayout layout(const sf::FloatRect &panel, std::size_t teamCount) const;

    /**
     * @brief Draw a 32x32 bordered texture stretched to an arbitrary rectangle
     *        using nine-slice scaling (corners kept, edges and center stretched).
     *
     * @param window      The window to draw into
     * @param texture     The 32x32 source panel texture
     * @param dest        The destination rectangle in window pixels
     */
    void drawNineSlice(sf::RenderWindow &window, const sf::Texture &texture, const sf::FloatRect &dest);

    bool _open = false; /* whether the settings panel is currently shown */
};
