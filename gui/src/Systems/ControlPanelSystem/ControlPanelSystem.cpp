/*
** EPITECH PROJECT, 2026
** gui/src/Systems/ControlPanelSystem/ControlPanelSystem
** File description:
** ControlPanelSystem source file
*/

#include "ControlPanelSystem.hpp"
#include "../../Components/Player.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Game/GameState.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Log.hpp"
#include "../../Protocol/GuiRequests.hpp"
#include "../../Protocol/NetworkClient.hpp"
#include "../../Resources/FontManager.hpp"
#include "../../Resources/TextureManager.hpp"
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <unordered_map>

/* Text colors tuned for dark lettering on the cream Kenney panel */
static const sf::Color CTRL_TITLE_COLOR(120, 68, 18);
static const sf::Color CTRL_VALUE_COLOR(74, 48, 28);
static const sf::Color CTRL_BTN_FILL_COLOR(214, 178, 120);
static const sf::Color CTRL_BTN_HOVER_FILL_COLOR(236, 206, 152);
static const sf::Color CTRL_BTN_BORDER_COLOR(120, 68, 18);
static const sf::Color CTRL_BTN_TEXT_COLOR(60, 38, 20);
/* Translucent white wash laid over a hovered button to brighten it */
static const sf::Color CTRL_HOVER_WASH_COLOR(255, 255, 255, 55);

/**
 * @brief Build a UI text with the shared outline style of the panel.
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
    return text;
}

/**
 * @brief Center a text horizontally and vertically inside a rectangle.
 *
 * @param text      The text to position (modified in place)
 * @param rect      The rectangle to center the text into
 */
static void centerText(sf::Text &text, const sf::FloatRect &rect)
{
    sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin({std::round(bounds.position.x + bounds.size.x / 2.0f), std::round(bounds.position.y + bounds.size.y / 2.0f)});
    text.setPosition({std::round(rect.position.x + rect.size.x / 2.0f), std::round(rect.position.y + rect.size.y / 2.0f)});
}

/**
 * @brief Compute the "Settings" toggle button rectangle (bottom-right).
 *
 * @param window      The window the button is drawn into
 *
 * @return The toggle button rectangle in window pixels.
 */
sf::FloatRect ControlPanelSystem::toggleRect(const sf::RenderWindow &window) const
{
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);

    return sf::FloatRect({winW - CTRL_PANEL_MARGIN - CTRL_TOGGLE_WIDTH, winH - CTRL_PANEL_MARGIN - CTRL_TOGGLE_HEIGHT},
                         {CTRL_TOGGLE_WIDTH, CTRL_TOGGLE_HEIGHT});
}

/**
 * @brief Compute the panel rectangle, anchored just above the toggle button.
 *
 * @param window      The window the panel is drawn into
 *
 * @return The panel rectangle in window pixels.
 */
sf::FloatRect ControlPanelSystem::panelRect(const sf::RenderWindow &window, std::size_t teamCount) const
{
    float infoLine = static_cast<float>(CTRL_PANEL_INFO_SIZE) + CTRL_PANEL_INFO_GAP;
    float infoHeight = static_cast<float>(2 + teamCount) * infoLine;
    float stepperRow = static_cast<float>(CTRL_PANEL_VALUE_SIZE) + CTRL_PANEL_TITLE_GAP + CTRL_BTN_SIZE;
    float height = 2.0f * CTRL_PANEL_PADDING + static_cast<float>(CTRL_PANEL_TITLE_SIZE) + CTRL_PANEL_TITLE_GAP +
                   infoHeight + 2.0f * stepperRow + CTRL_PANEL_GAP + CTRL_BTN_SIZE;
    sf::FloatRect toggle = toggleRect(window);
    float right = toggle.position.x + toggle.size.x;

    return sf::FloatRect({right - CTRL_PANEL_WIDTH, toggle.position.y - CTRL_PANEL_GAP - height},
                         {CTRL_PANEL_WIDTH, height});
}

/**
 * @brief Compute a left-to-right stepper row (-- - + ++) at a given Y.
 *
 * @param panel       The panel rectangle in window pixels
 * @param rowY        The top Y of the button row
 * @param steps       The delta applied by each button
 *
 * @return The four buttons laid out left to right.
 */
static std::array<FreqButton, 4> stepperRow(const sf::FloatRect &panel, float rowY,
                                            const std::array<int, 4> &steps)
{
    std::array<FreqButton, 4> result;
    float rowWidth = 4.0f * CTRL_BTN_SIZE + 3.0f * CTRL_BTN_GAP;
    float startX = panel.position.x + (panel.size.x - rowWidth) / 2.0f;

    for (std::size_t i = 0; i < result.size(); ++i) {
        result[i]._rect = sf::FloatRect({startX + static_cast<float>(i) * (CTRL_BTN_SIZE + CTRL_BTN_GAP), rowY},
                                        {CTRL_BTN_SIZE, CTRL_BTN_SIZE});
        result[i]._delta = steps[i];
        result[i]._label = FREQ_STEP_LABELS[i];
    }
    return result;
}

/**
 * @brief Compute the geometry of every control inside the panel.
 *
 * The controls stack below the info block: the frequency stepper, the music
 * volume stepper and finally a full-width "Force refresh" button.
 *
 * @param panel       The panel rectangle in window pixels
 * @param teamCount   The number of teams listed in the info block
 *
 * @return The pre-computed layout shared by drawing and click handling.
 */
PanelLayout ControlPanelSystem::layout(const sf::FloatRect &panel, std::size_t teamCount) const
{
    PanelLayout out;
    float infoLine = static_cast<float>(CTRL_PANEL_INFO_SIZE) + CTRL_PANEL_INFO_GAP;
    float valueGap = static_cast<float>(CTRL_PANEL_VALUE_SIZE) + CTRL_PANEL_TITLE_GAP;
    float y = panel.position.y + CTRL_PANEL_PADDING + static_cast<float>(CTRL_PANEL_TITLE_SIZE) +
              CTRL_PANEL_TITLE_GAP + static_cast<float>(2 + teamCount) * infoLine;

    out._freqValueY = y;
    y += valueGap;
    out._freqButtons = stepperRow(panel, y, FREQ_STEPS);
    y += CTRL_BTN_SIZE;
    out._volumeValueY = y;
    y += valueGap;
    out._volumeButtons = stepperRow(panel, y, VOLUME_STEPS);
    y += CTRL_BTN_SIZE + CTRL_PANEL_GAP;
    out._refreshRect = sf::FloatRect({panel.position.x + CTRL_PANEL_PADDING, y},
                                     {panel.size.x - 2.0f * CTRL_PANEL_PADDING, CTRL_BTN_SIZE});
    return out;
}

/**
 * @brief Draw a panel-styled button with a centered label.
 *
 * @param window      The window to draw into
 * @param texture     The 32x32 source panel texture
 * @param drawSlice   The nine-slice drawing routine to use for the background
 * @param font        The font used for the label
 * @param rect        The button rectangle in window pixels
 * @param text        The button label
 * @param hovered     Whether the mouse is currently over the button
 */
static void drawNineSliceButton(sf::RenderWindow &window, const sf::Texture &texture,
                                std::function<void(sf::RenderWindow &, const sf::Texture &, const sf::FloatRect &)> drawSlice,
                                const sf::Font &font, const sf::FloatRect &rect, const std::string &text, bool hovered)
{
    sf::Text label = makeText(font, text, CTRL_TOGGLE_FONT_SIZE, CTRL_TITLE_COLOR);

    drawSlice(window, texture, rect);
    if (hovered) {
        sf::RectangleShape wash({rect.size.x, rect.size.y});

        wash.setPosition(rect.position);
        wash.setFillColor(CTRL_HOVER_WASH_COLOR);
        window.draw(wash);
    }
    centerText(label, rect);
    window.draw(label);
}

/**
 * @brief Draw a row of stepper buttons (-- - + ++) with hover feedback.
 *
 * @param window      The window to draw into
 * @param font        The font used for the labels
 * @param row         The four buttons to draw
 * @param mouse       The mouse position in UI coordinates (for hover)
 */
static void drawStepperRow(sf::RenderWindow &window, const sf::Font &font,
                           const std::array<FreqButton, 4> &row, sf::Vector2f mouse)
{
    for (const FreqButton &button : row) {
        sf::RectangleShape box({button._rect.size.x, button._rect.size.y});
        sf::Text label = makeText(font, button._label, CTRL_BTN_FONT_SIZE, CTRL_BTN_TEXT_COLOR);
        bool hovered = button._rect.contains(mouse);

        box.setPosition(button._rect.position);
        box.setFillColor(hovered ? CTRL_BTN_HOVER_FILL_COLOR : CTRL_BTN_FILL_COLOR);
        box.setOutlineColor(CTRL_BTN_BORDER_COLOR);
        box.setOutlineThickness(hovered ? 3.0f : 2.0f);
        window.draw(box);
        centerText(label, button._rect);
        window.draw(label);
    }
}

/**
 * @brief Draw the "Settings" toggle button and, when open, the frequency panel.
 *
 * @param coordinator      The coordinator to access resources
 */
void ControlPanelSystem::update(Coordinator &coordinator)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    GameState &gameState = coordinator.getResource<GameState>();
    TextureManager &textures = coordinator.getResource<TextureManager>();
    const sf::Font &font = coordinator.getResource<FontManager>().get("chat");
    const sf::Texture &panelTex = textures.get("ui_panel");
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    auto drawSlice = [this](sf::RenderWindow &win, const sf::Texture &tex, const sf::FloatRect &dst) {
        drawNineSlice(win, tex, dst);
    };
    sf::FloatRect toggle = toggleRect(window);

    drawNineSliceButton(window, panelTex, drawSlice, font, toggle, _open ? "Close" : "Settings",
                        toggle.contains(mouse));
    if (_open == false) {
        return;
    }

    sf::FloatRect panel = panelRect(window, gameState._teams.size());

    drawNineSlice(window, panelTex, panel);

    float textX = panel.position.x + CTRL_PANEL_PADDING;
    sf::Text title = makeText(font, "Server settings", CTRL_PANEL_TITLE_SIZE, CTRL_TITLE_COLOR);
    title.setPosition({std::round(textX), std::round(panel.position.y + CTRL_PANEL_PADDING)});
    window.draw(title);

    float infoY = panel.position.y + CTRL_PANEL_PADDING + static_cast<float>(CTRL_PANEL_TITLE_SIZE) +
                  CTRL_PANEL_TITLE_GAP;
    sf::Text mapInfo =
        makeText(font, "Map: " + std::to_string(gameState._mapWidth) + " x " + std::to_string(gameState._mapHeight),
                 CTRL_PANEL_INFO_SIZE, CTRL_VALUE_COLOR);
    mapInfo.setPosition({std::round(textX), std::round(infoY)});
    window.draw(mapInfo);
    infoY += static_cast<float>(CTRL_PANEL_INFO_SIZE) + CTRL_PANEL_INFO_GAP;

    sf::Text teamsLabel = makeText(font, "Teams (" + std::to_string(gameState._teams.size()) + "):",
                                   CTRL_PANEL_INFO_SIZE, CTRL_VALUE_COLOR);
    teamsLabel.setPosition({std::round(textX), std::round(infoY)});
    window.draw(teamsLabel);
    infoY += static_cast<float>(CTRL_PANEL_INFO_SIZE) + CTRL_PANEL_INFO_GAP;

    std::unordered_map<std::string, std::uint32_t> playerCount;

    for (const auto &[id, entity] : gameState._players) {
        const Player &player = coordinator.getComponent<Player>(entity);

        playerCount[player._team] += 1;
    }

    for (const std::string &team : gameState._teams) {
        std::string line = "- " + team + " (" + std::to_string(playerCount[team]) + ")";
        sf::Text teamName = makeText(font, line, CTRL_PANEL_INFO_SIZE, teamColor(gameState._teams, team));

        teamName.setPosition({std::round(textX), std::round(infoY)});
        window.draw(teamName);
        infoY += static_cast<float>(CTRL_PANEL_INFO_SIZE) + CTRL_PANEL_INFO_GAP;
    }

    PanelLayout panelLayout = layout(panel, gameState._teams.size());

    sf::Text freqValue = makeText(font, "Frequency: " + std::to_string(gameState._freq), CTRL_PANEL_VALUE_SIZE,
                                  CTRL_VALUE_COLOR);
    freqValue.setPosition({std::round(textX), std::round(panelLayout._freqValueY)});
    window.draw(freqValue);
    drawStepperRow(window, font, panelLayout._freqButtons, mouse);

    sf::Text volumeValue = makeText(font, "Volume: " + std::to_string(static_cast<int>(gameState._musicVolume)),
                                    CTRL_PANEL_VALUE_SIZE, CTRL_VALUE_COLOR);
    volumeValue.setPosition({std::round(textX), std::round(panelLayout._volumeValueY)});
    window.draw(volumeValue);
    drawStepperRow(window, font, panelLayout._volumeButtons, mouse);

    drawNineSliceButton(window, panelTex, drawSlice, font, panelLayout._refreshRect, "Force refresh",
                        panelLayout._refreshRect.contains(mouse));
}

/**
 * @brief Route a left click (in UI coordinates) to the panel.
 *
 * @param coordinator      The coordinator to access resources
 * @param point            The click position in UI coordinates
 *
 * @return true if the click was inside the panel and thus consumed.
 */
bool ControlPanelSystem::handleClick(Coordinator &coordinator, sf::Vector2f point)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    GameState &gameState = coordinator.getResource<GameState>();

    if (toggleRect(window).contains(point)) {
        _open = !_open;
        return true;
    }
    if (_open == false) {
        return false;
    }

    sf::FloatRect panel = panelRect(window, gameState._teams.size());

    if (panel.contains(point) == false) {
        _open = false;
        return true;
    }

    PanelLayout panelLayout = layout(panel, gameState._teams.size());

    for (const FreqButton &button : panelLayout._freqButtons) {
        if (button._rect.contains(point) == false) {
            continue;
        }
        long target = static_cast<long>(gameState._freq) + button._delta;
        std::uint32_t newFreq = static_cast<std::uint32_t>(std::clamp(target, static_cast<long>(FREQ_MIN),
                                                                      static_cast<long>(FREQ_MAX)));

        if (newFreq != gameState._freq) {
            GuiRequests::setTimeUnit(coordinator.getResource<NetworkClient>(), newFreq);
            gameState._freq = newFreq;
            Log::info() << "Requested frequency change to " << newFreq << "\n";
        }
        return true;
    }
    for (const FreqButton &button : panelLayout._volumeButtons) {
        if (button._rect.contains(point) == false) {
            continue;
        }
        float newVolume = std::clamp(gameState._musicVolume + static_cast<float>(button._delta), VOLUME_MIN,
                                     VOLUME_MAX);

        if (newVolume != gameState._musicVolume) {
            gameState._musicVolume = newVolume;
            coordinator.getResource<sf::Music>().setVolume(newVolume);
            Log::info() << "Music volume set to " << static_cast<int>(newVolume) << "\n";
        }
        return true;
    }
    if (panelLayout._refreshRect.contains(point)) {
        GuiRequests::refreshAll(coordinator.getResource<NetworkClient>());
        Log::info() << "Requested a full state refresh from the server\n";
        return true;
    }
    return true;
}

/**
 * @brief Draw a 32x32 bordered texture stretched to an arbitrary rectangle
 *        using nine-slice scaling (corners kept, edges and center stretched).
 *
 * @param window      The window to draw into
 * @param texture     The 32x32 source panel texture
 * @param dest        The destination rectangle in window pixels
 */
void ControlPanelSystem::drawNineSlice(sf::RenderWindow &window, const sf::Texture &texture, const sf::FloatRect &dest)
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
