/*
 * EPITECH PROJECT, 2026
 * Constants.hpp
 * File description:
 * GUI constants
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

/* SFML window dimensions */
static constexpr int WIN_W = 800;
static constexpr int WIN_H = 800;

/* Pixel size of one game tile */
static constexpr int WORLD_TILE_PX = 16;

/* Width and height of one player animation frame (square) */
static constexpr int PLAYER_FRAME = 79;

static constexpr float REFERENCE_FREQ = 100.0f;
static constexpr float MIN_GAME_SPEED = 0.2f;
static constexpr float MAX_GAME_SPEED = 50.0f;

/* @brief Speed of the camera's movements */
static constexpr float CAMERA_SPEED = 300.f;

/* @brief Speed of the camera's zoom (fraction of zoom per second) */
static constexpr float ZOOM_SPEED = 1.5f;

/* Zoom factor bounds (smaller = closer, larger = further away) */
static constexpr float ZOOM_MIN = 0.3f;
static constexpr float ZOOM_MAX = 2.5f;

/* Scale applied to all sprites (players, eggs, decorations). */
static constexpr float ENTITY_SCALE = 0.5f;

/* Offsets that center a player on the middle of its tile. */
static constexpr int PLAYER_CONTENT_CENTER_Y = 32;
static constexpr int PLAYER_SPRITE_OFFSET_X = static_cast<int>(WORLD_TILE_PX / 2 - PLAYER_FRAME * ENTITY_SCALE / 2);
static constexpr int PLAYER_SPRITE_OFFSET_Y = static_cast<int>(WORLD_TILE_PX / 2 - PLAYER_CONTENT_CENTER_Y * ENTITY_SCALE);

/* Max pixel jitter (in each axis) applied so players on the same tile don't overlap. */
static constexpr int PLAYER_TILE_JITTER = WORLD_TILE_PX / 2;

/* Seconds a player takes to glide from one tile to the next. */
static constexpr float MOVE_INTERPOLATION_DURATION = 8 * 0.10f;

/* Seconds a dying player stays alive so its death animation can play out */
static constexpr float DEATH_ANIM_DURATION = 6 * 0.15f;

/* Ressources names in order */
static constexpr std::array<const char *, 7> RESOURCE_NAMES = {"food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"};

/* Angular speed (rad/s) of the pulsing aura worn by incanting players */
static constexpr float INCANTATION_GLOW_SPEED = 6.0f;

/* Render z-layers (lower = drawn first) */
static constexpr int Z_WORLD = 0;
static constexpr int Z_EGG = 1;
static constexpr int Z_PLAYER = 2;

/* Egg sprite size and offsets to center it on the middle of its tile. */
static constexpr int EGG_W = 9;
static constexpr int EGG_H = 10;
static constexpr int EGG_SPRITE_OFFSET_X = static_cast<int>(WORLD_TILE_PX / 2 - EGG_W * ENTITY_SCALE / 2);
static constexpr int EGG_SPRITE_OFFSET_Y = static_cast<int>(WORLD_TILE_PX / 2 - EGG_H * ENTITY_SCALE / 2);

/* Ressources */
static constexpr int RESSOURCES_SIZE = 16;

/* Chat overlay (broadcast messages, bottom-left of the window) */
static constexpr std::size_t CHAT_MAX_MESSAGES = 8;
static constexpr float CHAT_MSG_LIFETIME = 9.0f;
static constexpr float CHAT_FADE_DURATION = 1.5f;
static constexpr unsigned int CHAT_FONT_SIZE = 14;
static constexpr float CHAT_LINE_HEIGHT = 20.0f;
static constexpr float CHAT_MARGIN = 12.0f;
static constexpr float CHAT_BG_PADDING = 6.0f;
static constexpr std::uint8_t CHAT_BG_OPACITY = 150;
static constexpr std::uint8_t CHAT_BLUR_TINT = 90;
static constexpr float CHAT_BLUR_STRENGTH = 2.0f;

static constexpr float FOOD_SCALE = 0.5f;
static constexpr float MINERAL_SCALE = 0.75f;

/* Tile info panel (top-right overlay shown when a tile is clicked) */
static constexpr float INFO_PANEL_WIDTH = 210.0f;
static constexpr float INFO_PANEL_MARGIN = 12.0f;
static constexpr float INFO_PANEL_PADDING = 16.0f;
static constexpr float INFO_PANEL_ROW_HEIGHT = 24.0f;
static constexpr float INFO_PANEL_TITLE_GAP = 14.0f;
static constexpr float INFO_PANEL_ICON_SIZE = 16.0f;
static constexpr unsigned int INFO_PANEL_TITLE_SIZE = 18;
static constexpr unsigned int INFO_PANEL_FONT_SIZE = 15;
/* Source corner size (px) used to nine-slice the 32x32 panel texture */
static constexpr int INFO_PANEL_BORDER = 11;

/* End screen (full-window overlay shown when the server sends "seg <team>") */
static constexpr std::uint8_t END_SCREEN_DIM_ALPHA = 200;
static constexpr unsigned int END_SCREEN_TITLE_SIZE = 64;
static constexpr unsigned int END_SCREEN_TEAM_SIZE = 40;
static constexpr unsigned int END_SCREEN_HINT_SIZE = 18;
static constexpr float END_SCREEN_TITLE_GAP = 30.0f;
static constexpr float END_SCREEN_HINT_GAP = 40.0f;

/* Incantation levitation effect */
static constexpr float LEVITATE_AMPLITUDE = 2.0f;
static constexpr float LEVITATE_SPEED = 3.0f;

/* Expulsion shockwave particle effect */
static constexpr int SHOCKWAVE_PARTICLE_COUNT = 20;
static constexpr float SHOCKWAVE_PARTICLE_DURATION = 0.4f;
static constexpr float SHOCKWAVE_PARTICLE_SPEED = 55.f;
static constexpr float SHOCKWAVE_PARTICLE_SLOW = 0.88f;
static constexpr float SHOCKWAVE_PARTICLE_RADIUS = 1.5f;
static constexpr float SHOCKWAVE_RING_RADIUS = 6.f;

/* Teleport particle effect (toroidal wrap) */
static constexpr int TELEPORT_PARTICLE_COUNT = 14;
static constexpr float TELEPORT_PARTICLE_DURATION = 0.45f;
static constexpr float TELEPORT_PARTICLE_SPEED = 40.f;
static constexpr float TELEPORT_PARTICLE_SLOW = 0.92f;
static constexpr float TELEPORT_PARTICLE_RADIUS = 2.0f;
static constexpr float TELEPORT_RING_RADIUS = 6.0f;

/* Player highlight effect: the hovered/selected player is enlarged and tinted */
static constexpr float PLAYER_HOVER_SCALE = 1.5f;
static constexpr float PLAYER_SELECT_SCALE = 2.0f;
/* Amount each color channel is blended toward white (0 = none, 1 = full white) */
static constexpr float PLAYER_HOVER_TINT = 0.25f;
static constexpr float PLAYER_SELECT_TINT = 0.45f;
/* Minimum texture alpha (0-255) for a pixel to count as "on the player" when picking */
static constexpr std::uint8_t PLAYER_PICK_ALPHA = 64;

/* Control panel (bottom-right overlay to tweak the server frequency / time unit) */
static constexpr float CTRL_PANEL_WIDTH = 232.0f;
static constexpr float CTRL_PANEL_MARGIN = 12.0f;
static constexpr float CTRL_PANEL_PADDING = 16.0f;
static constexpr float CTRL_PANEL_TITLE_GAP = 14.0f;
static constexpr float CTRL_PANEL_GAP = 8.0f;
static constexpr float CTRL_BTN_SIZE = 32.0f;
static constexpr float CTRL_BTN_GAP = 8.0f;
static constexpr unsigned int CTRL_PANEL_TITLE_SIZE = 18;
static constexpr unsigned int CTRL_PANEL_VALUE_SIZE = 20;
static constexpr unsigned int CTRL_BTN_FONT_SIZE = 20;
/* Read-only info lines (map size + team list) shown above the frequency control */
static constexpr unsigned int CTRL_PANEL_INFO_SIZE = 16;
static constexpr float CTRL_PANEL_INFO_GAP = 8.0f;
/* The "Settings" toggle button anchored at the bottom-right corner */
static constexpr float CTRL_TOGGLE_WIDTH = 120.0f;
static constexpr float CTRL_TOGGLE_HEIGHT = 40.0f;
static constexpr unsigned int CTRL_TOGGLE_FONT_SIZE = 18;
/* Frequency bounds and the step applied by each button (-- - + ++) */
static constexpr std::uint32_t FREQ_MIN = 1;
static constexpr std::uint32_t FREQ_MAX = 10000;
static constexpr std::array<int, 4> FREQ_STEPS = {-100, -10, 10, 100};
static constexpr std::array<const char *, 4> FREQ_STEP_LABELS = {"--", "-", "+", "++"};
/* Music volume control: bounds, default and the step applied by each button (-- - + ++) */
static constexpr float MUSIC_DEFAULT_VOLUME = 50.0f;
static constexpr float VOLUME_MIN = 0.0f;
static constexpr float VOLUME_MAX = 100.0f;
static constexpr std::array<int, 4> VOLUME_STEPS = {-10, -1, 1, 10};
/* Tile lift effect: the hovered/selected tile is raised with a shader drop shadow */
static constexpr float TILE_LIFT_SCALE = 1.22f;
static constexpr float TILE_LIFT_RISE_RATIO = 0.16f;
static constexpr float TILE_LIFT_SPEED = 13.0f;
static constexpr float TILE_LIFT_SHADOW_SPREAD = 2.0f;
static constexpr float TILE_LIFT_SHADOW_DROP = 0.22f;
static constexpr float TILE_LIFT_SHADOW_BLUR = 2.4f;
static constexpr std::uint8_t TILE_LIFT_SHADOW_ALPHA = 120;
static constexpr std::uint8_t TILE_LIFT_BORDER_ALPHA = 110;
static constexpr float TILE_LIFT_BORDER_THICK = 2.0f;
