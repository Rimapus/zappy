/*
 * EPITECH PROJECT, 2026
 * GuiRequests.hpp
 * File description:
 * Typed helpers for every GUI command
 */

#pragma once

#include "NetworkClient.hpp"
#include <cstdint>

namespace GuiRequests {

/**
 * @brief Request the map dimensions (server replies with msz X Y).
 *
 * @param client The network client to send through
 */
void mapSize(NetworkClient &client);

/**
 * @brief Request the content of tile (x, y) (server replies with bct X Y q0..q6).
 *
 * @param client The network client to send through
 * @param x Tile column
 * @param y Tile row
 */
void tile(NetworkClient &client, std::uint32_t x, std::uint32_t y);

/**
 * @brief Request the content of every tile (server replies with one bct per tile).
 *
 * @param client The network client to send through
 */
void allTiles(NetworkClient &client);

/**
 * @brief Request all team names (server replies with one tna per team).
 *
 * @param client The network client to send through
 */
void teamNames(NetworkClient &client);

/**
 * @brief Request the position and orientation of a player (server replies with ppo).
 *
 * @param client The network client to send through
 * @param id Player id (must be >= 0)
 */
void playerPos(NetworkClient &client, int id);

/**
 * @brief Request the level of a player (server replies with plv).
 *
 * @param client The network client to send through
 * @param id Player id (must be >= 0)
 */
void playerLevel(NetworkClient &client, int id);

/**
 * @brief Request the inventory of a player (server replies with pin).
 *
 * @param client The network client to send through
 * @param id Player id (must be >= 0)
 */
void playerInventory(NetworkClient &client, int id);

/**
 * @brief Request the current server time unit (server replies with sgt T).
 *
 * @param client The network client to send through
 */
void timeUnit(NetworkClient &client);

/**
 * @brief Set the server time unit. t is clamped to [FREQ_MIN, FREQ_MAX].
 *
 * @param client The network client to send through
 * @param t The desired frequency
 */
void setTimeUnit(NetworkClient &client, std::uint32_t t);

/**
 * @brief Request a full state refresh: map size, all tiles, team names, time unit.
 *
 * @param client The network client to send through
 */
void refreshAll(NetworkClient &client);

} // namespace GuiRequests
