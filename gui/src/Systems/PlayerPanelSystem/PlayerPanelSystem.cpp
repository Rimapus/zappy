/*
** EPITECH PROJECT, 2026
** gui/src/Systems/PlayerPanelSystem/PlayerPanelSystem
** File description:
** PlayerPanelSystem source file
*/

#include "PlayerPanelSystem.hpp"
#include "../../Components/Inventory.hpp"
#include "../../Components/Player.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Game/GameState.hpp"
#include "../../Resources/FontManager.hpp"
#include "../../Resources/TextureManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cctype>
#include <string>

/* Text colors tuned for dark lettering on the cream Kenney panel */
static const sf::Color INFO_TITLE_COLOR(120, 68, 18);
static const sf::Color INFO_TEXT_COLOR(74, 48, 28);
static const sf::Color INFO_DIM_COLOR(170, 150, 124);
static const sf::Color INFO_OUTLINE_COLOR(247, 236, 210);

/**
 * @brief Capitalize the first letter of a resource name for display.
 *
 * @param name      The lowercase resource name
 *
 * @return The name with its first letter upper-cased.
 */
static std::string capitalize(const char *name)
{
    std::string label(name);

    if (label.empty() == false) {
        label[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(label[0])));
    }
    return label;
}

/**
 * @brief Build a UI text with the shared outline style of the info panel.
 *
 * @param font      The font to render with
 * @param str       The string to display
 * @param size      The character size
 * @param color     The fill color
 *
 * @return The configured text.
 */
static sf::Text makeText(const sf::Font &font, const std::string &str, unsigned int size, sf::Color color)
{
    sf::Text text(font, str, size);

    text.setFillColor(color);
    text.setOutlineColor(INFO_OUTLINE_COLOR);
    text.setOutlineThickness(1.0f);
    return text;
}

/**
 * @brief Draw a label followed by a right-aligned value on a single row.
 *
 * @param window      The window to draw into
 * @param font        The font used for the row text
 * @param label       The left-aligned label
 * @param value       The right-aligned value
 * @param panel       The panel rectangle in window pixels
 * @param rowY        The top y of the row in window pixels
 */
static void drawInfoRow(sf::RenderWindow &window, const sf::Font &font, const std::string &label,
                        const std::string &value, const sf::FloatRect &panel, float rowY)
{
    sf::Text left = makeText(font, label, INFO_PANEL_FONT_SIZE, INFO_DIM_COLOR);
    sf::Text right = makeText(font, value, INFO_PANEL_FONT_SIZE, INFO_TEXT_COLOR);

    left.setPosition({panel.position.x + INFO_PANEL_PADDING, rowY});
    window.draw(left);

    right.setPosition({panel.position.x + panel.size.x - INFO_PANEL_PADDING - right.getLocalBounds().size.x, rowY});
    window.draw(right);
}

/**
 * @brief Draw one resource row: icon, capitalized name and right-aligned count.
 *
 * @param window      The window to draw into
 * @param textures    The texture manager holding the resource icons
 * @param font        The font used for the row text
 * @param index       The resource index (0 = food, ...)
 * @param quantity    The resource quantity in the inventory
 * @param panel       The panel rectangle in window pixels
 * @param rowY        The top y of the row in window pixels
 */
static void drawResourceRow(sf::RenderWindow &window, TextureManager &textures, const sf::Font &font, int index,
                            std::uint32_t quantity, const sf::FloatRect &panel, float rowY)
{
    sf::Sprite icon(textures.get(std::string("ressource_") + RESOURCE_NAMES[index]));
    sf::Text label = makeText(font, capitalize(RESOURCE_NAMES[index]), INFO_PANEL_FONT_SIZE, INFO_TEXT_COLOR);
    sf::Text count = makeText(font, std::to_string(quantity), INFO_PANEL_FONT_SIZE,
                              quantity > 0 ? INFO_TEXT_COLOR : INFO_DIM_COLOR);
    float textY = rowY + (INFO_PANEL_ICON_SIZE - static_cast<float>(INFO_PANEL_FONT_SIZE)) / 2.0f;

    icon.setTextureRect(sf::IntRect({5 * RESSOURCES_SIZE, 0}, {RESSOURCES_SIZE, RESSOURCES_SIZE}));
    icon.setPosition({panel.position.x + INFO_PANEL_PADDING, rowY});
    window.draw(icon);

    label.setPosition({panel.position.x + INFO_PANEL_PADDING + INFO_PANEL_ICON_SIZE + 8.0f, textY});
    window.draw(label);

    count.setPosition({panel.position.x + panel.size.x - INFO_PANEL_PADDING - count.getLocalBounds().size.x, textY});
    window.draw(count);
}

/**
 * @brief Draw the inventory panel of the selected player (top-left UI overlay).
 *
 * @param coordinator      The coordinator to access resources and components
 */
void PlayerPanelSystem::update(Coordinator &coordinator)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    GameState &gameState = coordinator.getResource<GameState>();

    if (gameState._playerSelected == false) {
        return;
    }

    auto it = gameState._players.find(gameState._selectedPlayerId);
    if (it == gameState._players.end()) {
        gameState._playerSelected = false;
        return;
    }

    TextureManager &textures = coordinator.getResource<TextureManager>();
    const sf::Font &font = coordinator.getResource<FontManager>().get("chat");
    const Player &player = coordinator.getComponent<Player>(it->second);
    const Inventory &inv = coordinator.getComponent<Inventory>(it->second);

    float titleHeight = static_cast<float>(INFO_PANEL_TITLE_SIZE);
    float height = 2.0f * INFO_PANEL_PADDING + titleHeight + INFO_PANEL_TITLE_GAP +
                   INFO_PANEL_ROW_HEIGHT * static_cast<float>(2 + RESOURCE_NAMES.size());
    sf::FloatRect panel({INFO_PANEL_MARGIN, INFO_PANEL_MARGIN}, {INFO_PANEL_WIDTH, height});

    drawNineSlice(window, textures.get("ui_panel"), panel);

    sf::Text title = makeText(font, "Player #" + std::to_string(player._id), INFO_PANEL_TITLE_SIZE, INFO_TITLE_COLOR);
    title.setPosition({panel.position.x + INFO_PANEL_PADDING, panel.position.y + INFO_PANEL_PADDING});
    window.draw(title);

    float rowsTop = panel.position.y + INFO_PANEL_PADDING + titleHeight + INFO_PANEL_TITLE_GAP;
    drawInfoRow(window, font, "Team", player._team, panel, rowsTop);
    drawInfoRow(window, font, "Level", std::to_string(player._level), panel, rowsTop + INFO_PANEL_ROW_HEIGHT);

    float resourcesTop = rowsTop + INFO_PANEL_ROW_HEIGHT * 2.0f;
    for (std::size_t i = 0; i < RESOURCE_NAMES.size(); ++i) {
        drawResourceRow(window, textures, font, static_cast<int>(i), inv._resources[i], panel,
                        resourcesTop + INFO_PANEL_ROW_HEIGHT * static_cast<float>(i));
    }
}

/**
 * @brief Draw a 32x32 bordered texture stretched to an arbitrary rectangle
 *        using nine-slice scaling (corners kept, edges and center stretched).
 *
 * @param window      The window to draw into
 * @param texture     The 32x32 source panel texture
 * @param dest        The destination rectangle in window pixels
 */
void PlayerPanelSystem::drawNineSlice(sf::RenderWindow &window, const sf::Texture &texture, const sf::FloatRect &dest)
{
    int size = static_cast<int>(texture.getSize().x);
    int border = INFO_PANEL_BORDER;
    int center = size - 2 * border;
    float borderF = static_cast<float>(border);
    int srcX[3] = {0, border, size - border};
    int srcW[3] = {border, center, border};
    int srcY[3] = {0, border, size - border};
    int srcH[3] = {border, center, border};
    float dstX[3] = {dest.position.x, dest.position.x + borderF, dest.position.x + dest.size.x - borderF};
    float dstW[3] = {borderF, dest.size.x - 2.0f * borderF, borderF};
    float dstY[3] = {dest.position.y, dest.position.y + borderF, dest.position.y + dest.size.y - borderF};
    float dstH[3] = {borderF, dest.size.y - 2.0f * borderF, borderF};

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            sf::Sprite piece(texture, sf::IntRect({srcX[col], srcY[row]}, {srcW[col], srcH[row]}));

            piece.setPosition({dstX[col], dstY[row]});
            piece.setScale({dstW[col] / static_cast<float>(srcW[col]), dstH[row] / static_cast<float>(srcH[row])});
            window.draw(piece);
        }
    }
}
