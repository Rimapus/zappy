/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TileHighlightSystem/TileHighlightSystem
** File description:
** TileHighlightSystem source file
*/

#include "TileHighlightSystem.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Game/GameState.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <utility>

/* Resolution of the shadow render targets and size of the silhouette inside */
static constexpr unsigned int SHADOW_RT_SIZE = 128;
static constexpr float SHADOW_CORE_RATIO = 0.5f;

/* Separable 9-tap gaussian blur; run once per axis through a render target */
static const char *const BLUR_SHADER = R"(
uniform sampler2D texture;
uniform vec2 dir;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 sum = texture2D(texture, uv) * 0.227027;

    sum += texture2D(texture, uv + dir * 1.0) * 0.1945946;
    sum += texture2D(texture, uv - dir * 1.0) * 0.1945946;
    sum += texture2D(texture, uv + dir * 2.0) * 0.1216216;
    sum += texture2D(texture, uv - dir * 2.0) * 0.1216216;
    sum += texture2D(texture, uv + dir * 3.0) * 0.0540540;
    sum += texture2D(texture, uv - dir * 3.0) * 0.0540540;
    sum += texture2D(texture, uv + dir * 4.0) * 0.0162162;
    sum += texture2D(texture, uv - dir * 4.0) * 0.0162162;
    gl_FragColor = sum * gl_Color;
}
)";

/**
 * @brief Smoothstep easing for a nicer fade than a raw linear ramp.
 *
 * @param value      The input in [0, 1]
 *
 * @return The eased value in [0, 1].
 */
static float smoothstep(float value)
{
    float t = std::clamp(value, 0.0f, 1.0f);

    return t * t * (3.0f - 2.0f * t);
}

/**
 * @brief Tell whether (x, y) is one of the currently wanted tiles.
 *
 * @param wanted      The wanted tiles
 * @param count       The number of valid entries in wanted
 * @param x           The tile x coordinate
 * @param y           The tile y coordinate
 *
 * @return true if the tile is wanted (should ease toward fully lifted).
 */
static bool isWanted(const std::array<std::pair<std::uint32_t, std::uint32_t>, 2> &wanted, std::size_t count,
                     std::uint32_t x, std::uint32_t y)
{
    for (std::size_t i = 0; i < count; ++i) {
        if (wanted[i].first == x && wanted[i].second == y) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Smoothly lift the hovered and selected tiles out of the board.
 *
 * @param coordinator      The coordinator to access resources and components
 * @param dt               Elapsed time since the last frame (seconds)
 */
void TileHighlightSystem::update(Coordinator &coordinator, float dt)
{
    GameState &gameState = coordinator.getResource<GameState>();
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    float ease = std::min(1.0f, dt * TILE_LIFT_SPEED);
    std::array<std::pair<std::uint32_t, std::uint32_t>, 2> wanted;
    std::size_t wantedCount = 0;

    if (gameState._tileHovered == true) {
        wanted[wantedCount++] = {gameState._hoveredTileX, gameState._hoveredTileY};
    }
    if (gameState._tileSelected == true) {
        wanted[wantedCount++] = {gameState._selectedTileX, gameState._selectedTileY};
    }
    for (std::size_t i = 0; i < wantedCount; ++i) {
        bool known = false;
        for (LiftState &entry : _lifts) {
            known = known || (entry._x == wanted[i].first && entry._y == wanted[i].second);
        }
        if (known == false) {
            _lifts.push_back({wanted[i].first, wanted[i].second, 0.0f});
        }
    }
    for (LiftState &entry : _lifts) {
        float target = isWanted(wanted, wantedCount, entry._x, entry._y) ? 1.0f : 0.0f;
        entry._lift += (target - entry._lift) * ease;
    }
    _lifts.erase(std::remove_if(_lifts.begin(), _lifts.end(),
                                [&](const LiftState &e) {
                                    return e._lift < 0.005f && isWanted(wanted, wantedCount, e._x, e._y) == false;
                                }),
                 _lifts.end());

    gameState._tileLift.clear();
    for (const LiftState &entry : _lifts) {
        gameState._tileLift[{entry._x, entry._y}] = smoothstep(entry._lift);
    }

    bool anyVisible = false;
    for (const LiftState &entry : _lifts) {
        anyVisible = anyVisible || entry._lift > 0.01f;
    }
    if (anyVisible == false) {
        return;
    }

    sf::Vector2u windowSize = window.getSize();
    if (_screen.getSize() != windowSize && _screen.resize(windowSize) == false) {
        return;
    }
    _screen.update(window);
    ensureBlur();

    sf::View camera = window.getView();
    sf::Vector2f windowSizeF(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));

    window.setView(sf::View(sf::FloatRect({0.0f, 0.0f}, windowSizeF)));
    for (const LiftState &entry : _lifts) {
        if (entry._lift > 0.01f) {
            liftTile(window, camera, coordinator, entry._x, entry._y, smoothstep(entry._lift));
        }
    }
    for (const LiftState &entry : _lifts) {
        if (entry._lift > 0.01f) {
            drawRim(window, camera, coordinator, entry._x, entry._y, smoothstep(entry._lift));
        }
    }
    window.setView(camera);
}

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
bool TileHighlightSystem::computeLift(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator,
                                      std::uint32_t x, std::uint32_t y, float amount, sf::IntRect &region,
                                      sf::Vector2f &topLeft, float &scale)
{
    GameState &gameState = coordinator.getResource<GameState>();
    auto it = gameState._tiles.find({x, y});

    if (it == gameState._tiles.end()) {
        return false;
    }

    Transform &transform = coordinator.getComponent<Transform>(it->second);
    sf::Vector2f tileTL(static_cast<float>(transform._x), static_cast<float>(transform._y));
    sf::Vector2f tileBR(tileTL.x + WORLD_TILE_PX, tileTL.y + WORLD_TILE_PX);
    sf::Vector2i screenTL = window.mapCoordsToPixel(tileTL, camera);
    sf::Vector2i screenBR = window.mapCoordsToPixel(tileBR, camera);
    sf::Vector2u windowSize = window.getSize();

    if (screenTL.x < 0 || screenTL.y < 0 || screenBR.x > static_cast<int>(windowSize.x) ||
        screenBR.y > static_cast<int>(windowSize.y) || screenBR.x <= screenTL.x || screenBR.y <= screenTL.y) {
        return false;
    }
    region = sf::IntRect(screenTL, {screenBR.x - screenTL.x, screenBR.y - screenTL.y});
    topLeft = sf::Vector2f(static_cast<float>(screenTL.x), static_cast<float>(screenTL.y));
    scale = 1.0f + (TILE_LIFT_SCALE - 1.0f) * amount;
    return true;
}

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
void TileHighlightSystem::liftTile(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator,
                                   std::uint32_t x, std::uint32_t y, float amount)
{
    sf::IntRect region;
    sf::Vector2f topLeft;
    float scale = 1.0f;

    if (computeLift(window, camera, coordinator, x, y, amount, region, topLeft, scale) == false) {
        return;
    }

    float w = static_cast<float>(region.size.x);
    float h = static_cast<float>(region.size.y);
    sf::Vector2f center(topLeft.x + w / 2.0f, topLeft.y + h / 2.0f);
    sf::Sprite lifted(_screen, region);

    if (_blurReady == true) {
        float rtSize = static_cast<float>(SHADOW_RT_SIZE);
        float shadowSize = h * TILE_LIFT_SHADOW_SPREAD * (0.85f + 0.15f * amount);
        sf::Sprite shadow(_shadowA.getTexture());

        buildShadow(region);
        shadow.setOrigin({rtSize / 2.0f, rtSize / 2.0f});
        shadow.setScale({shadowSize / rtSize, shadowSize / rtSize});
        shadow.setPosition({center.x, center.y + h * TILE_LIFT_SHADOW_DROP * amount});
        shadow.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(TILE_LIFT_SHADOW_ALPHA * amount)));
        window.draw(shadow);
    }

    lifted.setOrigin({w / 2.0f, h / 2.0f});
    lifted.setScale({scale, scale});
    lifted.setPosition({center.x, center.y - h * TILE_LIFT_RISE_RATIO * amount});
    window.draw(lifted);
}

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
void TileHighlightSystem::drawRim(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator,
                                  std::uint32_t x, std::uint32_t y, float amount)
{
    sf::IntRect region;
    sf::Vector2f topLeft;
    float scale = 1.0f;

    if (computeLift(window, camera, coordinator, x, y, amount, region, topLeft, scale) == false) {
        return;
    }

    float w = static_cast<float>(region.size.x);
    float h = static_cast<float>(region.size.y);
    sf::Vector2f center(topLeft.x + w / 2.0f, topLeft.y + h / 2.0f);
    sf::RectangleShape rim({w * scale, h * scale});

    rim.setOrigin({w * scale / 2.0f, h * scale / 2.0f});
    rim.setPosition({center.x, center.y - h * TILE_LIFT_RISE_RATIO * amount});
    rim.setFillColor(sf::Color(0, 0, 0, 0));
    rim.setOutlineColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(TILE_LIFT_BORDER_ALPHA * amount)));
    rim.setOutlineThickness(TILE_LIFT_BORDER_THICK);
    window.draw(rim);
}

/**
 * @brief Render the gaussian-blurred shadow of a tile region into _shadowA.
 *
 * @param tile      The tile region in the framebuffer snapshot
 */
void TileHighlightSystem::buildShadow(const sf::IntRect &tile)
{
    float half = static_cast<float>(SHADOW_RT_SIZE) / 2.0f;
    float core = static_cast<float>(SHADOW_RT_SIZE) * SHADOW_CORE_RATIO;
    float texel = TILE_LIFT_SHADOW_BLUR / static_cast<float>(SHADOW_RT_SIZE);
    sf::Sprite silhouette(_screen, tile);
    sf::RenderStates states;

    silhouette.setOrigin({static_cast<float>(tile.size.x) / 2.0f, static_cast<float>(tile.size.y) / 2.0f});
    silhouette.setScale({core / static_cast<float>(tile.size.x), core / static_cast<float>(tile.size.y)});
    silhouette.setPosition({half, half});
    silhouette.setColor(sf::Color(0, 0, 0, 255));
    _shadowA.clear(sf::Color(0, 0, 0, 0));
    _shadowA.draw(silhouette);
    _shadowA.display();

    states.shader = &_blur;
    _blur.setUniform("texture", sf::Shader::CurrentTexture);
    _blur.setUniform("dir", sf::Glsl::Vec2(texel, 0.0f));
    sf::Sprite horizontal(_shadowA.getTexture());
    _shadowB.clear(sf::Color(0, 0, 0, 0));
    _shadowB.draw(horizontal, states);
    _shadowB.display();

    _blur.setUniform("dir", sf::Glsl::Vec2(0.0f, texel));
    sf::Sprite vertical(_shadowB.getTexture());
    _shadowA.clear(sf::Color(0, 0, 0, 0));
    _shadowA.draw(vertical, states);
    _shadowA.display();
}

/**
 * @brief Lazily compile the blur shader and allocate the shadow targets.
 *
 * @return true if the shader pipeline is ready, false if unsupported.
 */
bool TileHighlightSystem::ensureBlur()
{
    if (_blurTried == true) {
        return _blurReady;
    }
    _blurTried = true;
    if (sf::Shader::isAvailable() == false) {
        return false;
    }
    if (_blur.loadFromMemory(BLUR_SHADER, sf::Shader::Type::Fragment) == false) {
        return false;
    }
    if (_shadowA.resize({SHADOW_RT_SIZE, SHADOW_RT_SIZE}) == false ||
        _shadowB.resize({SHADOW_RT_SIZE, SHADOW_RT_SIZE}) == false) {
        return false;
    }
    _shadowA.setSmooth(true);
    _shadowB.setSmooth(true);
    _blurReady = true;
    return true;
}
