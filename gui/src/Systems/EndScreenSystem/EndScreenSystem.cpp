/*
** EPITECH PROJECT, 2026
** gui/src/Systems/EndScreenSystem
** File description:
** EndScreenSystem source file
*/

#include "EndScreenSystem.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Game/GameState.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Resources/FontManager.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

/**
 * @brief Build a centered, outlined text label.
 *
 * @param font The font to render with
 * @param str The string to display
 * @param size The character size
 * @param color The fill color
 *
 * @return A ready-to-position sf::Text with its origin at its center.
 */
static sf::Text makeCenteredText(const sf::Font &font, const std::string &str, unsigned int size, sf::Color color)
{
    sf::Text text(font, str, size);
    sf::FloatRect bounds = text.getLocalBounds();

    text.setFillColor(color);
    text.setOutlineColor(sf::Color(0, 0, 0, 220));
    text.setOutlineThickness(2.0f);
    text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
    return text;
}

/**
 * @brief Dim the screen and draw the "Press Escape to quit" hint.
 *
 * @param window The window to draw into
 * @param font The font to render with
 *
 * @return The window center in pixels.
 */
sf::Vector2f EndScreenSystem::drawBase(sf::RenderWindow &window, const sf::Font &font) const
{
    sf::Vector2f size(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    sf::Vector2f center(size.x / 2.0f, size.y / 2.0f);

    sf::RectangleShape dim(size);
    dim.setFillColor(sf::Color(0, 0, 0, END_SCREEN_DIM_ALPHA));
    window.draw(dim);

    sf::Text hint = makeCenteredText(font, "Press Escape to quit", END_SCREEN_HINT_SIZE, sf::Color(180, 180, 180));
    hint.setPosition({center.x, center.y + END_SCREEN_TITLE_GAP + END_SCREEN_HINT_GAP + END_SCREEN_TEAM_SIZE / 2.0f});
    window.draw(hint);

    return center;
}

/**
 * @brief Draw the "GAME OVER — <team> wins!" overlay.
 *
 * @param window The window to draw into
 * @param font The font to render with
 * @param center The screen center returned by drawBase
 * @param winnerTeam The name of the winning team
 * @param winColor The team color
 */
void EndScreenSystem::drawGameOver(sf::RenderWindow &window, const sf::Font &font, sf::Vector2f center,
                                   const std::string &winnerTeam, sf::Color winColor) const
{
    sf::Text title = makeCenteredText(font, "GAME OVER", END_SCREEN_TITLE_SIZE, sf::Color::White);
    title.setPosition({center.x, center.y - END_SCREEN_TITLE_GAP - END_SCREEN_TEAM_SIZE / 2.0f});
    window.draw(title);

    sf::Text team = makeCenteredText(font, winnerTeam + " wins!", END_SCREEN_TEAM_SIZE, winColor);
    team.setPosition({center.x, center.y + END_SCREEN_TITLE_GAP});
    window.draw(team);
}

/**
 * @brief Draw the "CONNECTION LOST" overlay.
 *
 * @param window The window to draw into
 * @param font The font to render with
 * @param center The screen center returned by drawBase
 */
void EndScreenSystem::drawDisconnected(sf::RenderWindow &window, const sf::Font &font, sf::Vector2f center) const
{
    sf::Text title = makeCenteredText(font, "CONNECTION LOST", END_SCREEN_TITLE_SIZE, sf::Color(220, 60, 60));
    title.setPosition({center.x, center.y - END_SCREEN_TITLE_GAP - END_SCREEN_TEAM_SIZE / 2.0f});
    window.draw(title);

    sf::Text sub = makeCenteredText(font, "Lost connection to the server", END_SCREEN_TEAM_SIZE, sf::Color(200, 200, 200));
    sub.setPosition({center.x, center.y + END_SCREEN_TITLE_GAP});
    window.draw(sub);
}

/**
 * @brief Draw the appropriate end overlay if the game has ended or the
 * connection was lost. No-op while the game is still running.
 *
 * @param coordinator The coordinator to access resources
 */
void EndScreenSystem::update(Coordinator &coordinator)
{
    GameState &gameState = coordinator.getResource<GameState>();

    if (gameState._gameOver == false && gameState._disconnected == false) {
        return;
    }

    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    const sf::Font &font = coordinator.getResource<FontManager>().get("chat");
    sf::Vector2f center = drawBase(window, font);

    if (gameState._gameOver) {
        sf::Color winColor = teamColor(gameState._teams, gameState._winnerTeam);
        drawGameOver(window, font, center, gameState._winnerTeam, winColor);
    } else {
        drawDisconnected(window, font, center);
    }
}
