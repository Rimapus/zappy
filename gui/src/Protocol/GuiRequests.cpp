/*
 * EPITECH PROJECT, 2026
 * GuiRequests.cpp
 * File description:
 * Typed helpers for every GUI outgoing command
 */

#include "GuiRequests.hpp"
#include "../Constants.hpp"
#include "../Log.hpp"
#include <algorithm>
#include <string>

namespace GuiRequests {

namespace {

bool checkConnected(const NetworkClient &client, const char *cmd)
{
    if (client.isConnected()) {
        return true;
    }
    Log::warn() << "GuiRequests::" << cmd << ": not connected, request dropped\n";
    return false;
}

std::string playerId(int id)
{
    return "#" + std::to_string(id);
}

} // namespace

/**
 * @brief Request the map dimensions.
 *
 * @param client The network client to send through
 */
void mapSize(NetworkClient &client)
{
    if (!checkConnected(client, "mapSize")) {
        return;
    }
    client.send("msz");
}

/**
 * @brief Request the content of tile (x, y).
 *
 * @param client The network client to send through
 * @param x Tile column
 * @param y Tile row
 */
void tile(NetworkClient &client, std::uint32_t x, std::uint32_t y)
{
    if (!checkConnected(client, "tile")) {
        return;
    }
    client.send("bct " + std::to_string(x) + " " + std::to_string(y));
}

/**
 * @brief Request the content of every tile.
 *
 * @param client The network client to send through
 */
void allTiles(NetworkClient &client)
{
    if (!checkConnected(client, "allTiles")) {
        return;
    }
    client.send("mct");
}

/**
 * @brief Request all team names.
 *
 * @param client The network client to send through
 */
void teamNames(NetworkClient &client)
{
    if (!checkConnected(client, "teamNames")) {
        return;
    }
    client.send("tna");
}

/**
 * @brief Request the position and orientation of a player.
 *
 * @param client The network client to send through
 * @param id Player id (must be >= 0)
 */
void playerPos(NetworkClient &client, int id)
{
    if (!checkConnected(client, "playerPos")) {
        return;
    }
    if (id < 0) {
        Log::warn() << "GuiRequests::playerPos: invalid player id " << id << "\n";
        return;
    }
    client.send("ppo " + playerId(id));
}

/**
 * @brief Request the level of a player.
 *
 * @param client The network client to send through
 * @param id Player id (must be >= 0)
 */
void playerLevel(NetworkClient &client, int id)
{
    if (!checkConnected(client, "playerLevel")) {
        return;
    }
    if (id < 0) {
        Log::warn() << "GuiRequests::playerLevel: invalid player id " << id << "\n";
        return;
    }
    client.send("plv " + playerId(id));
}

/**
 * @brief Request the inventory of a player.
 *
 * @param client The network client to send through
 * @param id Player id (must be >= 0)
 */
void playerInventory(NetworkClient &client, int id)
{
    if (!checkConnected(client, "playerInventory")) {
        return;
    }
    if (id < 0) {
        Log::warn() << "GuiRequests::playerInventory: invalid player id " << id << "\n";
        return;
    }
    client.send("pin " + playerId(id));
}

/**
 * @brief Request the current server time unit.
 *
 * @param client The network client to send through
 */
void timeUnit(NetworkClient &client)
{
    if (!checkConnected(client, "timeUnit")) {
        return;
    }
    client.send("sgt");
}

/**
 * @brief Set the server time unit. t is clamped to [FREQ_MIN, FREQ_MAX].
 *
 * @param client The network client to send through
 * @param t The desired frequency
 */
void setTimeUnit(NetworkClient &client, std::uint32_t t)
{
    if (!checkConnected(client, "setTimeUnit")) {
        return;
    }
    std::uint32_t clamped = std::clamp(t, FREQ_MIN, FREQ_MAX);
    if (clamped != t) {
        Log::warn() << "GuiRequests::setTimeUnit: frequency " << t << " clamped to " << clamped << "\n";
    }
    client.send("sst " + std::to_string(clamped));
}

/**
 * @brief Request a full state refresh: map size, all tiles, team names, time unit.
 *
 * @param client The network client to send through
 */
void refreshAll(NetworkClient &client)
{
    if (!checkConnected(client, "refreshAll")) {
        return;
    }
    mapSize(client);
    allTiles(client);
    teamNames(client);
    timeUnit(client);
}

} // namespace GuiRequests
