/*
** EPITECH PROJECT, 2026
** gui/src/main
** File description:
** MAIN source file
*/

#include "Args.hpp"
#include "Components/Animator.hpp"
#include "Components/Background.hpp"
#include "Components/Glow.hpp"
#include "Components/Inventory.hpp"
#include "Components/Levitate.hpp"
#include "Components/Lifetime.hpp"
#include "Components/Movement.hpp"
#include "Components/Offsets.hpp"
#include "Components/Particle.hpp"
#include "Components/Sprite.hpp"
#include "Components/Tile.hpp"
#include "Components/TileMap.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"
#include "Constants.hpp"
#include "ECS/Coordinator/Coordinator.hpp"
#include "Game/GameState.hpp"
#include "Protocol/GuiRequests.hpp"
#include "Protocol/Handler.hpp"
#include "Protocol/NetworkClient.hpp"
#include "Protocol/Parser.hpp"
#include "Resources/FontManager.hpp"
#include "Resources/TextureManager.hpp"
#include "Systems/AnimatorSystem/AnimatorSystem.hpp"
#include "Systems/BackgroundSystem/BackgroundSystem.hpp"
#include "Systems/ChatSystem/ChatSystem.hpp"
#include "Systems/ControlPanelSystem/ControlPanelSystem.hpp"
#include "Systems/EndScreenSystem/EndScreenSystem.hpp"
#include "Systems/GlowSystem/GlowSystem.hpp"
#include "Systems/LevitateSystem/LevitateSystem.hpp"
#include "Systems/LifetimeSystem/LifetimeSystem.hpp"
#include "Systems/MovementSystem/MovementSystem.hpp"
#include "Systems/ParticleSystem/ParticleSystem.hpp"
#include "Systems/PlayerPanelSystem/PlayerPanelSystem.hpp"
#include "Systems/RenderSystem/RenderSystem.hpp"
#include "Systems/TileHighlightSystem/TileHighlightSystem.hpp"
#include "Systems/TileMapSystem/TileMapSystem.hpp"
#include "Systems/TilePanelSystem/TilePanelSystem.hpp"
#include "Systems/TileResourceSystem/TileResourceSystem.hpp"
#include "Tiles/TileSet.hpp"
#include "World/WorldGenerator.hpp"
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <random>

/**
 * @brief Load every resource and texture the GUI needs.
 *
 * @param coordinator      The coordinator to populate
 */
static void setupResources(Coordinator &coordinator)
{
    TextureManager &textures = coordinator.addResource<TextureManager>();
    FontManager &fonts = coordinator.addResource<FontManager>();

    coordinator.addResource<sf::RenderWindow>(sf::VideoMode({800, 800}), "Zappy GUI");
    GameState &gameState = coordinator.addResource<GameState>();
    coordinator.addResource<TileSet>();

    gameState._musicVolume = MUSIC_DEFAULT_VOLUME;

    sf::Music &music = coordinator.addResource<sf::Music>();
    if (music.openFromFile("assets/Music/main.ogg")) {
        music.setLooping(true);
        music.setVolume(gameState._musicVolume);
        music.play();
    } else {
        std::cerr << "Warning: could not load assets/Music/main.ogg\n";
    }
    fonts.load("chat", "assets/Fonts/8bitOperatorPlus-Regular.ttf");
    textures.load("player_idle", "assets/Player/idle.png");
    textures.load("player_walk", "assets/Player/walk.png");
    textures.load("player_death", "assets/Player/death.png");
    textures.load("player_hurt", "assets/Player/hurt.png");
    textures.load("egg", "assets/Egg/egg.png");
    textures.load("ui_panel", "assets/UI/panel.png");
    textures.load("ressource_food", "assets/Ressources/food.png");
    textures.load("ressource_linemate", "assets/Ressources/linemate.png");
    textures.load("ressource_deraumere", "assets/Ressources/deraumere.png");
    textures.load("ressource_sibur", "assets/Ressources/sibur.png");
    textures.load("ressource_mendiane", "assets/Ressources/mendiane.png");
    textures.load("ressource_phiras", "assets/Ressources/phiras.png");
    textures.load("ressource_thystame", "assets/Ressources/thystame.png");
}

/**
 * @brief Register the ECS systems and wire their component signatures.
 *
 * @param coordinator      The coordinator to register the systems into
 * @param render           Receives the RenderSystem
 * @param animator         Receives the AnimatorSystem
 * @param tileMap          Receives the TileMapSystem
 * @param background       Receives the BackgroundSystem
 * @param lifetime         Receives the LifetimeSystem
 * @param movement         Receives the MovementSystem
 * @param glow             Receives the GlowSystem
 * @param chat             Receives the ChatSystem
 * @param tileResource     Receives the TileResourceSystem
 * @param levitate         Receives the LevitateSystem
 */
static void setupSystems(Coordinator &coordinator, std::shared_ptr<RenderSystem> &render,
                         std::shared_ptr<AnimatorSystem> &animator, std::shared_ptr<TileMapSystem> &tileMap,
                         std::shared_ptr<BackgroundSystem> &background, std::shared_ptr<LifetimeSystem> &lifetime,
                         std::shared_ptr<MovementSystem> &movement, std::shared_ptr<GlowSystem> &glow,
                         std::shared_ptr<ChatSystem> &chat, std::shared_ptr<TileResourceSystem> &tileResource,
                         std::shared_ptr<TilePanelSystem> &tilePanel,
                         std::shared_ptr<TileHighlightSystem> &tileHighlight,
                         std::shared_ptr<ParticleSystem> &particle, std::shared_ptr<PlayerPanelSystem> &playerPanel,
                         std::shared_ptr<LevitateSystem> &levitate, std::shared_ptr<ControlPanelSystem> &controlPanel,
                         std::shared_ptr<EndScreenSystem> &endScreen)
{
    Signature signature = 0;

    // Render System
    render = coordinator.registerSystem<RenderSystem>();
    signature.set(coordinator.getComponentType<Transform>(), true);
    signature.set(coordinator.getComponentType<Sprite>(), true);
    coordinator.setSystemSignature<RenderSystem>(signature);

    // Animator System
    signature = 0;
    animator = coordinator.registerSystem<AnimatorSystem>();
    signature.set(coordinator.getComponentType<Animator>(), true);
    signature.set(coordinator.getComponentType<Sprite>(), true);
    coordinator.setSystemSignature<AnimatorSystem>(signature);

    // TileMap System
    signature = 0;
    tileMap = coordinator.registerSystem<TileMapSystem>();
    signature.set(coordinator.getComponentType<Transform>(), true);
    signature.set(coordinator.getComponentType<TileMap>(), true);
    coordinator.setSystemSignature<TileMapSystem>(signature);

    // Background System
    signature = 0;
    background = coordinator.registerSystem<BackgroundSystem>();
    signature.set(coordinator.getComponentType<Background>(), true);
    coordinator.setSystemSignature<BackgroundSystem>(signature);

    // Lifetime System
    signature = 0;
    lifetime = coordinator.registerSystem<LifetimeSystem>();
    signature.set(coordinator.getComponentType<Lifetime>(), true);
    coordinator.setSystemSignature<LifetimeSystem>(signature);

    // Movement System (interpolates entities between tiles)
    signature = 0;
    movement = coordinator.registerSystem<MovementSystem>();
    signature.set(coordinator.getComponentType<Transform>(), true);
    signature.set(coordinator.getComponentType<Movement>(), true);
    coordinator.setSystemSignature<MovementSystem>(signature);

    // Glow System (pulses the tint of incanting players)
    signature = 0;
    glow = coordinator.registerSystem<GlowSystem>();
    signature.set(coordinator.getComponentType<Glow>(), true);
    signature.set(coordinator.getComponentType<Sprite>(), true);
    coordinator.setSystemSignature<GlowSystem>(signature);

    // Chat System (renders broadcast messages, drives no entities)
    signature = 0;
    chat = coordinator.registerSystem<ChatSystem>();
    coordinator.setSystemSignature<ChatSystem>(signature);

    // Tile Panel System (renders the selected tile's info, drives no entities)
    signature = 0;
    tilePanel = coordinator.registerSystem<TilePanelSystem>();
    coordinator.setSystemSignature<TilePanelSystem>(signature);

    // Tile Highlight System (outlines the hovered/selected tiles, drives no entities)
    signature = 0;
    tileHighlight = coordinator.registerSystem<TileHighlightSystem>();
    coordinator.setSystemSignature<TileHighlightSystem>(signature);

    // Player Panel System (renders the selected player's inventory, drives no entities)
    signature = 0;
    playerPanel = coordinator.registerSystem<PlayerPanelSystem>();
    coordinator.setSystemSignature<PlayerPanelSystem>(signature);

    // Control Panel System (server-parameter overlay, drives no entities)
    signature = 0;
    controlPanel = coordinator.registerSystem<ControlPanelSystem>();
    coordinator.setSystemSignature<ControlPanelSystem>(signature);

    // End Screen System (game-over overlay, drives no entities)
    signature = 0;
    endScreen = coordinator.registerSystem<EndScreenSystem>();
    coordinator.setSystemSignature<EndScreenSystem>(signature);

    // Tile Resource System (draws mineral sprites on game tiles)
    signature = 0;
    tileResource = coordinator.registerSystem<TileResourceSystem>();
    signature.set(coordinator.getComponentType<Transform>(), true);
    signature.set(coordinator.getComponentType<Inventory>(), true);
    signature.set(coordinator.getComponentType<Tile>(), true);
    signature.set(coordinator.getComponentType<Offsets>(), true);
    coordinator.setSystemSignature<TileResourceSystem>(signature);

    // Particle System (short-lived visual effects: teleport bursts, etc.)
    signature = 0;
    particle = coordinator.registerSystem<ParticleSystem>();
    signature.set(coordinator.getComponentType<Transform>(), true);
    signature.set(coordinator.getComponentType<Particle>(), true);
    signature.set(coordinator.getComponentType<Velocity>(), true);
    signature.set(coordinator.getComponentType<Lifetime>(), true);
    coordinator.setSystemSignature<ParticleSystem>(signature);

    // Levitate System (bobs incanting players up and down)
    signature = 0;
    levitate = coordinator.registerSystem<LevitateSystem>();
    signature.set(coordinator.getComponentType<Levitate>(), true);
    coordinator.setSystemSignature<LevitateSystem>(signature);
}

/**
 * @brief Read everything pending on the socket and dispatch it to the handler.
 *
 * @param client       The network client to poll
 * @param handler      The command handler to feed
 */
static void readNetwork(NetworkClient &client, Handler &handler)
{
    for (const std::string &line : client.poll()) {
        if (line == "WELCOME") {
            client.send("GRAPHIC");
            continue;
        }
        handler.handle(Parser::parse(line));
    }
}

/**
 * @brief Move the camera with the ZQSD keys, framerate independent.
 *
 * @param view      The view to move (the camera)
 * @param dt        Elapsed time since last frame, in seconds
 */
static void updateCamera(sf::View &view, float dt)
{
    sf::Vector2f move(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
        move.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        move.y += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
        move.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        move.x += 1.0f;
    }
    view.move(move * CAMERA_SPEED * dt);
}

/**
 * @brief Zoom the camera in (A) or out (E), framerate independent and clamped.
 *
 * @param view      The view to zoom (the camera)
 * @param dt        Elapsed time since last frame, in seconds
 * @param zoom      The current cumulative zoom factor, updated in place
 */
static void updateZoom(sf::View &view, float dt, float &zoom)
{
    float factor = 1.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        factor -= ZOOM_SPEED * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
        factor += ZOOM_SPEED * dt;
    }
    float newZoom = std::clamp(zoom * factor, ZOOM_MIN, ZOOM_MAX);

    if (newZoom != zoom) {
        view.zoom(newZoom / zoom);
        zoom = newZoom;
    }
}

/**
 * @brief Keep the camera center inside the island bounds (border clamp).
 *
 * @param view       The camera view to clamp
 * @param bounds     The island bounds in world pixels
 */
static void clampCamera(sf::View &view, const sf::FloatRect &bounds)
{
    float left = bounds.position.x;
    float top = bounds.position.y;
    float right = left + bounds.size.x;
    float bottom = top + bounds.size.y;
    sf::Vector2f center = view.getCenter();

    center.x = std::clamp(center.x, left, right);
    center.y = std::clamp(center.y, top, bottom);
    view.setCenter(center);
}

/**
 * @brief Find the tile under a pixel, if any.
 *
 * @param window       The window the pixel belongs to
 * @param camera       The world camera used to map the pixel to world space
 * @param coordinator  The coordinator to access tile components
 * @param gameState    The game state holding the tiles
 * @param pixel        The pixel in window coordinates
 * @param outX         Receives the tile x coordinate on success
 * @param outY         Receives the tile y coordinate on success
 *
 * @return true if a tile sits under the pixel, false otherwise.
 */
static bool findTileAt(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator,
                       GameState &gameState, sf::Vector2i pixel, std::uint32_t &outX, std::uint32_t &outY)
{
    sf::Vector2f world = window.mapPixelToCoords(pixel, camera);

    for (auto const &[coord, entity] : gameState._tiles) {
        Transform &transform = coordinator.getComponent<Transform>(entity);
        float left = static_cast<float>(transform._x);
        float top = static_cast<float>(transform._y);

        if (world.x >= left && world.x < left + WORLD_TILE_PX && world.y >= top && world.y < top + WORLD_TILE_PX) {
            outX = coord.first;
            outY = coord.second;
            return true;
        }
    }
    return false;
}

/**
 * @brief Tell whether a world point lands on an opaque pixel of a player sprite.
 *
 * The 79x79 player frames are mostly transparent padding, so a bounding-box
 * test would match well beside the visible character. We map the point into
 * the sprite's current animation frame and sample the texture alpha there, so
 * only a hit on the drawn body counts. The sprite's own transform is skipped
 * (it gets mutated by the render highlight); the authoritative Transform and
 * the fixed ENTITY_SCALE are used instead.
 *
 * @param coordinator  The coordinator to access the entity components
 * @param entity       The player entity to test
 * @param world        The point in world coordinates
 *
 * @return true if the point lands on an opaque pixel of the player.
 */
static bool playerPixelHit(Coordinator &coordinator, Entity entity, sf::Vector2f world)
{
    static std::map<const sf::Texture *, sf::Image> imageCache;
    Sprite &sprite = coordinator.getComponent<Sprite>(entity);

    if (sprite._sprite.has_value() == false) {
        return false;
    }

    Transform &transform = coordinator.getComponent<Transform>(entity);
    sf::IntRect rect = sprite._sprite->getTextureRect();
    float localX = (world.x - static_cast<float>(transform._x)) / ENTITY_SCALE;
    float localY = (world.y - static_cast<float>(transform._y)) / ENTITY_SCALE;

    if (localX < 0.0f || localY < 0.0f || localX >= static_cast<float>(rect.size.x) ||
        localY >= static_cast<float>(rect.size.y)) {
        return false;
    }

    const sf::Texture &texture = sprite._sprite->getTexture();
    auto it = imageCache.find(&texture);
    if (it == imageCache.end()) {
        it = imageCache.emplace(&texture, texture.copyToImage()).first;
    }

    sf::Vector2u texel(static_cast<unsigned int>(rect.position.x + static_cast<int>(localX)),
                       static_cast<unsigned int>(rect.position.y + static_cast<int>(localY)));
    if (texel.x >= it->second.getSize().x || texel.y >= it->second.getSize().y) {
        return false;
    }
    return it->second.getPixel(texel).a >= PLAYER_PICK_ALPHA;
}

/**
 * @brief Find the player whose drawn body sits under a pixel, if any.
 *
 * @param window       The window the pixel belongs to
 * @param camera       The world camera used to map the pixel to world space
 * @param coordinator  The coordinator to access player components
 * @param gameState    The game state holding the players
 * @param pixel        The pixel in window coordinates
 * @param outId        Receives the player id on success
 *
 * @return true if a player sits under the pixel, false otherwise.
 */
static bool findPlayerAt(sf::RenderWindow &window, const sf::View &camera, Coordinator &coordinator,
                         GameState &gameState, sf::Vector2i pixel, std::uint32_t &outId)
{
    sf::Vector2f world = window.mapPixelToCoords(pixel, camera);

    for (auto const &[id, entity] : gameState._players) {
        if (playerPixelHit(coordinator, entity, world)) {
            outId = id;
            return true;
        }
    }
    return false;
}

/**
 * @brief Poll events and do updates.
 *
 * @param window	 The window reference
 * @param uiView	 UI View reference
 * @param camera	 Camera view reference
 * @param zoom	 Current camera zoom factor (kept on resize)
 * @param coordinator	 The coordinator, to resolve tile clicks
 * @param gameState	 The game state, updated with the tile selection
 */
static void updateEvents(sf::RenderWindow &window, sf::View &uiView, sf::View &camera, float zoom,
                         Coordinator &coordinator, GameState &gameState, ControlPanelSystem &controlPanel)
{
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
        }
        if (const auto *resized = event->getIf<sf::Event::Resized>()) {
            sf::Vector2f size(static_cast<float>(resized->size.x),
                              static_cast<float>(resized->size.y));

            camera.setSize(size);
            camera.zoom(zoom);
            uiView.setSize(size);
            uiView.setCenter(size / 2.f);
        }
        if (const auto *clicked = event->getIf<sf::Event::MouseButtonPressed>()) {
            std::uint32_t tx = 0;
            std::uint32_t ty = 0;
            std::uint32_t pid = 0;

            if (clicked->button == sf::Mouse::Button::Left) {
                if (controlPanel.handleClick(coordinator, window.mapPixelToCoords(clicked->position, uiView))) {
                    continue;
                }
                if (findPlayerAt(window, camera, coordinator, gameState, clicked->position, pid)) {
                    gameState._playerSelected = true;
                    gameState._selectedPlayerId = pid;
                    NetworkClient &client = coordinator.getResource<NetworkClient>();
                    GuiRequests::playerPos(client, static_cast<int>(pid));
                    GuiRequests::playerLevel(client, static_cast<int>(pid));
                    GuiRequests::playerInventory(client, static_cast<int>(pid));
                }
                if (findTileAt(window, camera, coordinator, gameState, clicked->position, tx, ty)) {
                    gameState._tileSelected = true;
                    gameState._selectedTileX = tx;
                    gameState._selectedTileY = ty;
                    GuiRequests::tile(coordinator.getResource<NetworkClient>(), tx, ty);
                }
            }
        }
        if (const auto *moved = event->getIf<sf::Event::MouseMoved>()) {
            std::uint32_t tx = 0;
            std::uint32_t ty = 0;
            std::uint32_t pid = 0;

            gameState._playerHovered =
                findPlayerAt(window, camera, coordinator, gameState, moved->position, pid);
            gameState._hoveredPlayerId = pid;
            gameState._tileHovered = findTileAt(window, camera, coordinator, gameState, moved->position, tx, ty);
            gameState._hoveredTileX = tx;
            gameState._hoveredTileY = ty;
        }
    }
}

/**
 * @brief MAIN entry point of the program.
 *
 * @param argc        	 ARGC
 * @param char *argv[]	 ARGV
 *
 * @return Main return of the GUI.
 */
int main(int argc, char *argv[])
{
    Coordinator coordinator;
    GuiArgs args;

    if (Args::parse(argc, argv, args) == false) {
        Args::usage(argv[0]);
        return 84;
    }
    try {
        setupResources(coordinator);
    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize resources: " << e.what() << std::endl;
        return 84;
    }
    NetworkClient &client = coordinator.addResource<NetworkClient>();

    try {
        client.connect(args._host, args._port);
    } catch (const std::exception &e) {
        std::cerr << "Failed to connect to " << args._host << ":" << args._port << ": " << e.what() << std::endl;
        return 84;
    }

    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();
    GameState &gameState = coordinator.getResource<GameState>();
    Handler handler(coordinator);
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<AnimatorSystem> animatorSystem;
    std::shared_ptr<TileMapSystem> tileMapSystem;
    std::shared_ptr<BackgroundSystem> backgroundSystem;
    std::shared_ptr<LifetimeSystem> lifetimeSystem;
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<GlowSystem> glowSystem;
    std::shared_ptr<ChatSystem> chatSystem;
    std::shared_ptr<TileResourceSystem> tileResourceSystem;
    std::shared_ptr<TilePanelSystem> tilePanelSystem;
    std::shared_ptr<TileHighlightSystem> tileHighlightSystem;
    std::shared_ptr<ParticleSystem> particleSystem;
    std::shared_ptr<PlayerPanelSystem> playerPanelSystem;
    std::shared_ptr<ControlPanelSystem> controlPanelSystem;
    std::shared_ptr<LevitateSystem> levitateSystem;
    std::shared_ptr<EndScreenSystem> endScreenSystem;

    setupSystems(coordinator, renderSystem, animatorSystem, tileMapSystem, backgroundSystem, lifetimeSystem,
                 movementSystem, glowSystem, chatSystem, tileResourceSystem, tilePanelSystem, tileHighlightSystem,
                 particleSystem, playerPanelSystem, levitateSystem, controlPanelSystem, endScreenSystem);

    bool worldGenerated = false;
    sf::FloatRect islandBounds;
    sf::Clock clock;
    sf::View camera = window.getDefaultView();
    sf::View uiView = window.getDefaultView();
    float zoom = 1.0f;
    while (window.isOpen() == true) {
        float dt = clock.restart().asSeconds();
        float gameSpeed = gameState._freq > 0
                              ? std::clamp(static_cast<float>(gameState._freq) / REFERENCE_FREQ,
                                           MIN_GAME_SPEED, MAX_GAME_SPEED)
                              : 1.0f;
        float gameDt = dt * gameSpeed;

        updateEvents(window, uiView, camera, zoom, coordinator, gameState, *controlPanelSystem);
        updateCamera(camera, dt);
        updateZoom(camera, dt, zoom);
        if (worldGenerated == true) {
            clampCamera(camera, islandBounds);
        }
        if (gameState._disconnected == false) {
            try {
                readNetwork(client, handler);
            } catch (const std::exception &e) {
                std::cerr << "Connection lost: " << e.what() << std::endl;
                gameState._disconnected = true;
            }
        }
        if (worldGenerated == false && gameState._mapWidth > 0 && gameState._mapHeight > 0) {
            try {
                std::random_device seedSource;
                sf::Vector2u winSize = window.getSize();
                WorldGenerator generator(seedSource(), static_cast<int>(gameState._mapWidth),
                                         static_cast<int>(gameState._mapHeight),
                                         static_cast<int>(winSize.x), static_cast<int>(winSize.y));

                generator.generate(coordinator);
                islandBounds = generator.islandBounds();
                camera.setCenter(generator.islandCenter());
                worldGenerated = true;
            } catch (const std::exception &e) {
                std::cerr << "Failed to generate the world: " << e.what() << std::endl;
                window.close();
                break;
            }
        }
        window.clear();
        lifetimeSystem->update(coordinator, gameDt);
        animatorSystem->update(coordinator, gameDt);
        movementSystem->update(coordinator, gameDt);
        levitateSystem->update(coordinator, gameDt);
        glowSystem->update(coordinator, gameDt);
        window.setView(uiView);
        backgroundSystem->update(coordinator);
        window.setView(camera);
        tileMapSystem->update(coordinator);
        renderSystem->update(coordinator, Z_WORLD, Z_WORLD);
        tileResourceSystem->update(coordinator);
        tileHighlightSystem->update(coordinator, dt);
        renderSystem->update(coordinator, Z_EGG, Z_PLAYER);
        particleSystem->update(coordinator, gameDt);
        window.setView(uiView);
        chatSystem->update(coordinator, dt);
        tilePanelSystem->update(coordinator);
        playerPanelSystem->update(coordinator);
        controlPanelSystem->update(coordinator);
        endScreenSystem->update(coordinator);
        window.display();
    }
    return 0;
}
