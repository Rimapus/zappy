/*
 * EPITECH PROJECT, 2026
 * onPnw.cpp
 * File description:
 * pnw command handler
 */

#include "../../Components/Animator.hpp"
#include "../../Components/Inventory.hpp"
#include "../../Components/Movement.hpp"
#include "../../Components/Offsets.hpp"
#include "../../Components/Player.hpp"
#include "../../Components/Sprite.hpp"
#include "../../Components/TileAnchor.hpp"
#include "../../Components/Transform.hpp"
#include "../../Constants.hpp"
#include "../../Game/TeamColor.hpp"
#include "../../Log.hpp"
#include "../../Resources/TextureManager.hpp"
#include "../Handler.hpp"
#include "../PlayerOrientation.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <random>

/**
 * @brief Pick a small random pixel offset inside [-PLAYER_TILE_JITTER, +PLAYER_TILE_JITTER].
 *
 * Lets several players sharing a tile spread out a little instead of stacking
 * on the exact same pixel.
 *
 * @return std::int32_t The random offset to add on one axis.
 */
static std::int32_t tileJitter()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(-PLAYER_TILE_JITTER, PLAYER_TILE_JITTER);

    return static_cast<std::int32_t>(dist(rng));
}

/**
 * @brief Build a vector of frames for an animation clip from a spritesheet row.
 *
 * @param row The spritesheet row index (0-3)
 * @param count The number of frames in that row
 * @return std::vector<sf::IntRect> The list of frame sub-rects for that animation
 */
static std::vector<sf::IntRect> makeFrames(int row, int count)
{
    std::vector<sf::IntRect> frames;
    for (int i = 0; i < count; ++i) {
        frames.emplace_back(sf::IntRect({i * PLAYER_FRAME, row * PLAYER_FRAME}, {PLAYER_FRAME, PLAYER_FRAME}));
    }
    return frames;
}

/**
 * @brief Build the animator clips for a player entity.
 *
 * @param animator The Animator to populate with clips
 */
static void buildClips(Animator &animator)
{
    for (int o = 1; o <= 4; ++o) {
        std::string d = orientName(o);
        int row = orientRow(o);
        animator._clips["idle_" + d] = {makeFrames(row, 4), 0.20f, true, "player_idle"};
        animator._clips["walk_" + d] = {makeFrames(row, 8), 0.10f, false, "player_walk"};
        animator._clips["death_" + d] = {makeFrames(row, 6), 0.15f, false, "player_death"};
        animator._clips["hurt_" + d] = {makeFrames(row, 4), 0.12f, false, "player_hurt"};
    }
}

/**
 * @brief Handle pnw: spawn a new player entity in the ECS.
 *
 * @param cmd args: #n, X, Y, O (orientation 1-4), L (level), N (team)
 */
void Handler::onPnw(const Command &cmd)
{
    int playerId;
    int x;
    int y;
    int orientation;
    int level;
    std::string teamName;

    if (cmd._args.size() != 6) {
        Log::error() << "Invalid pnw command: expected 6 arguments, got " << cmd._args.size() << "\n";
        return;
    }
    if (sscanf(cmd._args[0].c_str(), "#%d", &playerId) != 1) {
        Log::error() << "Invalid pnw command: invalid player id format: " << cmd._args[0] << "\n";
        return;
    }
    try {
        x = std::stoi(cmd._args[1]);
        y = std::stoi(cmd._args[2]);
        orientation = std::stoi(cmd._args[3]);
        level = std::stoi(cmd._args[4]);
        teamName = cmd._args[5];
    } catch (const std::exception &e) {
        Log::error() << "Invalid pnw command: invalid argument format: " << e.what() << "\n";
        return;
    }
    if (x < 0 || y < 0 || (_gameState._mapWidth > 0 && (static_cast<std::uint32_t>(x) >= _gameState._mapWidth || static_cast<std::uint32_t>(y) >= _gameState._mapHeight))) {
        Log::error() << "Invalid pnw command: position (" << x << "," << y << ") out of bounds\n";
        return;
    }
    if (orientation < 1 || orientation > 4) {
        Log::error() << "Invalid pnw command: orientation out of range: " << orientation << "\n";
        return;
    }
    if (level < 1 || level > 8) {
        Log::error() << "Invalid pnw command: level out of range: " << level << "\n";
        return;
    }
    auto &teams = _gameState._teams;
    if (!teams.empty() && std::find(teams.begin(), teams.end(), teamName) == teams.end()) {
        Log::error() << "Invalid pnw command: unknown team \"" << teamName << "\"\n";
        return;
    }
    if (_gameState._players.count(playerId)) {
        Log::error() << "Invalid pnw command: player " << playerId << " already exists\n";
        return;
    }

    TextureManager &textures = _coordinator.getResource<TextureManager>();
    Entity playerEntity = _coordinator.createEntity();

    sf::Sprite sprite(textures.get("player_idle"));
    sprite.setTextureRect(sf::IntRect({0, orientRow(orientation) * PLAYER_FRAME}, {PLAYER_FRAME, PLAYER_FRAME}));
    sprite.setColor(teamColor(_gameState._teams, teamName));

    Animator animator;
    buildClips(animator);
    animator._current = "idle_" + orientName(orientation);
    animator._playing = true;

    Offsets offsets;
    offsets._x[0] = static_cast<float>(tileJitter());
    offsets._y[0] = static_cast<float>(tileJitter());

    _coordinator.addComponent(playerEntity, Transform{
                                                _gameState._tileOriginX + x * WORLD_TILE_PX + PLAYER_SPRITE_OFFSET_X + static_cast<std::int32_t>(offsets._x[0]),
                                                _gameState._tileOriginY + y * WORLD_TILE_PX + PLAYER_SPRITE_OFFSET_Y + static_cast<std::int32_t>(offsets._y[0]),
                                                Z_PLAYER});
    _coordinator.addComponent(playerEntity, Sprite{sprite});
    _coordinator.addComponent(playerEntity, animator);
    _coordinator.addComponent(playerEntity, Player{
                                                static_cast<std::uint32_t>(playerId),
                                                static_cast<std::uint32_t>(orientation),
                                                static_cast<std::uint32_t>(level),
                                                teamName});
    _coordinator.addComponent(playerEntity, TileAnchor{static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)});
    _coordinator.addComponent(playerEntity, offsets);
    _coordinator.addComponent(playerEntity, Inventory{});
    _coordinator.addComponent(playerEntity, Movement{});

    _gameState._players[playerId] = playerEntity;
    Log::info() << "Player " << playerId << " connected at (" << x << "," << y
                << ") facing " << orientation << " level " << level << " team " << teamName << "\n";
}
