/*
** EPITECH PROJECT, 2026
** gui/src/Systems/ChatSystem
** File description:
** ChatSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

namespace sf {
class RenderWindow;
}

/* @brief System that renders broadcast messages in the bottom-left corner */
class ChatSystem : public System
{
public:
    /**
     * @brief Age out stale broadcast messages and draw the live ones in the
     *        bottom-left corner of the window (fixed UI overlay).
     *
     * @param coordinator      The coordinator to access resources
     * @param dt               Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);

private:
    /**
     * @brief Lazily compile the blur shader on first use.
     *
     * @return true if the shader is ready, false if blur is unsupported
     */
    bool ensureBlur();

    /**
     * @brief Grab the panel region of the framebuffer and draw it blurred,
     *        used as a frosted-glass backdrop behind the chat lines.
     *
     * @param window      The window to capture and draw into
     * @param panel       The panel rectangle in window pixels
     * @param alpha       The fade alpha applied to the backdrop
     */
    void drawBlur(sf::RenderWindow &window, const sf::FloatRect &panel, std::uint8_t alpha);

    sf::Texture _screen;     /* framebuffer snapshot used as the blur source */
    sf::Shader _blur;        /* two-axis box blur shader */
    bool _blurTried = false; /* whether shader compilation was attempted */
    bool _blurReady = false; /* whether the shader compiled successfully */
};
