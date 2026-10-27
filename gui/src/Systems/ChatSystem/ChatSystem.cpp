/*
** EPITECH PROJECT, 2026
** gui/src/Systems/ChatSystem/ChatSystem
** File description:
** ChatSystem source file
*/

#include "ChatSystem.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Game/GameState.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Resources/FontManager.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <algorithm>
#include <vector>

/* @brief Single-pass box blur sampling a 5x5 grid around each pixel. */
static const char *const BLUR_FRAGMENT_SHADER = R"(
uniform sampler2D texture;
uniform vec2 offset;

void main()
{
    vec4 sum = vec4(0.0);

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            sum += texture2D(texture, gl_TexCoord[0].xy + vec2(float(x), float(y)) * offset);
        }
    }
    gl_FragColor = vec4((sum / 25.0).rgb, 1.0) * gl_Color;
}
)";

/**
 * @brief Drop every broadcast message that has outlived CHAT_MSG_LIFETIME.
 *
 * @param chat      The chat buffer to age, in place
 * @param dt        Elapsed time since the last frame (seconds)
 */
static void ageMessages(std::deque<ChatMessage> &chat, float dt)
{
    for (ChatMessage &msg : chat) {
        msg._age += dt;
    }
    while (chat.empty() == false && chat.front()._age >= CHAT_MSG_LIFETIME) {
        chat.pop_front();
    }
}

/**
 * @brief Fade a message out over its last CHAT_FADE_DURATION seconds.
 *
 * @param age      The age of the message (seconds)
 *
 * @return The alpha value in [0, 255].
 */
static std::uint8_t fadeAlpha(float age)
{
    float remaining = CHAT_MSG_LIFETIME - age;

    if (remaining >= CHAT_FADE_DURATION) {
        return 255;
    }
    return static_cast<std::uint8_t>(std::clamp(remaining / CHAT_FADE_DURATION, 0.0f, 1.0f) * 255.0f);
}

/**
 * @brief Age out stale broadcast messages and draw the live ones in the
 *        bottom-left corner of the window (fixed UI overlay).
 *
 * @param coordinator      The coordinator to access resources
 * @param dt               Elapsed time since the last frame (seconds)
 */
void ChatSystem::update(Coordinator &coordinator, float dt)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    GameState &gameState = coordinator.getResource<GameState>();
    FontManager &fonts = coordinator.getResource<FontManager>();

    ageMessages(gameState._chat, dt);
    if (gameState._chat.empty() == true) {
        return;
    }

    const sf::Font &font = fonts.get("chat");
    float bottom = static_cast<float>(window.getSize().y) - CHAT_MARGIN;
    std::size_t count = gameState._chat.size();

    std::vector<sf::Text> texts;
    float maxWidth = 0.0f;
    std::uint8_t panelAlpha = 0;

    texts.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const ChatMessage &msg = gameState._chat[count - 1 - i];
        sf::Color color = msg._isServer ? sf::Color(180, 180, 180) : teamColor(gameState._teams, msg._team);
        std::uint8_t alpha = fadeAlpha(msg._age);
        std::string prefix = msg._isServer ? "[server]: " : "#" + std::to_string(msg._playerId) + ": ";
        sf::Text text(font, prefix + msg._text, CHAT_FONT_SIZE);

        color.a = alpha;
        text.setFillColor(color);
        text.setOutlineColor(sf::Color(0, 0, 0, alpha));
        text.setOutlineThickness(1.0f);
        text.setPosition({CHAT_MARGIN, bottom - CHAT_LINE_HEIGHT * static_cast<float>(i + 1)});
        maxWidth = std::max(maxWidth, text.getLocalBounds().size.x);
        panelAlpha = std::max(panelAlpha, alpha);
        texts.push_back(std::move(text));
    }

    float top = bottom - CHAT_LINE_HEIGHT * static_cast<float>(count);
    sf::FloatRect panelRect(
        {CHAT_MARGIN - CHAT_BG_PADDING, top - CHAT_BG_PADDING},
        {maxWidth + 2.0f * CHAT_BG_PADDING, CHAT_LINE_HEIGHT * static_cast<float>(count) + 2.0f * CHAT_BG_PADDING});

    if (ensureBlur() == true) {
        drawBlur(window, panelRect, panelAlpha);
    } else {
        sf::RectangleShape panel(panelRect.size);

        panel.setPosition(panelRect.position);
        panel.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(panelAlpha * CHAT_BG_OPACITY / 255)));
        window.draw(panel);
    }

    for (const sf::Text &text : texts) {
        window.draw(text);
    }
}

/**
 * @brief Lazily compile the blur shader on first use.
 *
 * @return true if the shader is ready, false if blur is unsupported
 */
bool ChatSystem::ensureBlur()
{
    if (_blurTried == true) {
        return _blurReady;
    }
    _blurTried = true;
    if (sf::Shader::isAvailable() == false) {
        return false;
    }
    _blurReady = _blur.loadFromMemory(BLUR_FRAGMENT_SHADER, sf::Shader::Type::Fragment);
    return _blurReady;
}

/**
 * @brief Grab the panel region of the framebuffer and draw it blurred,
 *        used as a frosted-glass backdrop behind the chat lines.
 *
 * @param window      The window to capture and draw into
 * @param panel       The panel rectangle in window pixels
 * @param alpha       The fade alpha applied to the backdrop
 */
void ChatSystem::drawBlur(sf::RenderWindow &window, const sf::FloatRect &panel, std::uint8_t alpha)
{
    sf::Vector2u windowSize = window.getSize();

    if (_screen.getSize() != windowSize && _screen.resize(windowSize) == false) {
        return;
    }
    _screen.update(window);

    int left = std::clamp(static_cast<int>(panel.position.x), 0, static_cast<int>(windowSize.x));
    int top = std::clamp(static_cast<int>(panel.position.y), 0, static_cast<int>(windowSize.y));
    int right = std::clamp(static_cast<int>(panel.position.x + panel.size.x), 0, static_cast<int>(windowSize.x));
    int bottom = std::clamp(static_cast<int>(panel.position.y + panel.size.y), 0, static_cast<int>(windowSize.y));
    sf::IntRect region({left, top}, {right - left, bottom - top});

    if (region.size.x <= 0 || region.size.y <= 0) {
        return;
    }

    sf::Sprite backdrop(_screen, region);
    sf::RenderStates states;

    backdrop.setPosition({static_cast<float>(left), static_cast<float>(top)});
    backdrop.setColor(sf::Color(255, 255, 255, alpha));
    _blur.setUniform("texture", sf::Shader::CurrentTexture);
    _blur.setUniform("offset",
                     sf::Glsl::Vec2(CHAT_BLUR_STRENGTH / static_cast<float>(windowSize.x),
                                    CHAT_BLUR_STRENGTH / static_cast<float>(windowSize.y)));
    states.shader = &_blur;
    window.draw(backdrop, states);

    sf::RectangleShape tint(panel.size);

    tint.setPosition(panel.position);
    tint.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha * CHAT_BLUR_TINT / 255)));
    window.draw(tint);
}
