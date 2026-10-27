/*
** EPITECH PROJECT, 2026
** Look.cpp
** File description:
** Look command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Player.hpp"
#include "map/Map.hpp"
#include "map/Tile.hpp"
#include <array>
#include <cstddef>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace server {

struct Vector2D
{
    long long _x;
    long long _y;
};

static constexpr std::array<Vector2D, 4> FORWARD_OFFSETS{
    {
     {0, -1},
     {1, 0},
     {0, 1},
     {-1, 0},
     }
};

static constexpr std::array<Vector2D, 4> LEFT_OFFSETS{
    {
     {-1, 0},
     {0, -1},
     {1, 0},
     {0, 1},
     }
};

static std::size_t wrapCoordinate(long long value, std::size_t limit)
{
    if (limit == 0) {
        return 0;
    }
    const long long mod = static_cast<long long>(limit);
    return static_cast<std::size_t>(((value % mod) + mod) % mod);
}

static void appendTileContents(const Tile &tile, std::size_t playerCount, std::string &result)
{
    bool firstEntry = true;

    auto append = [&](std::string_view s) {
        if (!firstEntry) {
            result.push_back(' ');
        }
        result.append(s);
        firstEntry = false;
    };

    for (std::size_t i = 0; i < playerCount; i++) {
        append("player");
    }

    for (const auto &resource : tile.getResources()) {
        append(resource.toString());
    }
}

static std::vector<std::size_t> buildPlayerCounts(const ServerData &data)
{
    const auto &map = data.getMap();
    std::vector<std::size_t> playerCounts(map.width() * map.height(), 0);

    for (const auto &player : data.getClientsData()._activeClients) {
        if (!player) {
            continue;
        }
        const auto x = player->getPlayer().getX();
        const auto y = player->getPlayer().getY();
        if (x < map.width() && y < map.height()) {
            playerCounts[y * map.width() + x]++;
        }
    }

    return playerCounts;
}

static std::string buildLookReply(ServerData &data, const std::shared_ptr<Client> &client)
{
    const auto &player = client->getPlayer();
    const auto &map = data.getMap();

    if (map.width() == 0 || map.height() == 0) {
        return "[]";
    }

    const auto playerCounts = buildPlayerCounts(data);
    const auto directionIndex = static_cast<std::size_t>(player.getDirection());
    const auto forward = FORWARD_OFFSETS[directionIndex];
    const auto left = LEFT_OFFSETS[directionIndex];
    const Vector2D right{-left._x, -left._y};
    const long long baseX = static_cast<long long>(player.getX());
    const long long baseY = static_cast<long long>(player.getY());
    const auto level = player.getLevel();

    auto getTileInfo = [&map, &playerCounts](long long tx, long long ty) -> std::pair<const Tile &, std::size_t> {
        const auto x = wrapCoordinate(tx, map.width());
        const auto y = wrapCoordinate(ty, map.height());
        return {map.at(x, y), playerCounts[y * map.width() + x]};
    };

    std::string result = "[";

    for (std::size_t line = 0; line <= level; line++) {
        const long long distance = static_cast<long long>(line);
        const long long rowStartX = baseX + (forward._x + left._x) * distance;
        const long long rowStartY = baseY + (forward._y + left._y) * distance;
        const long long tileCount = 2 * distance + 1;

        for (long long i = 0; i < tileCount; i++) {
            const auto [tile, playerCount] = getTileInfo(
                rowStartX + right._x * i,
                rowStartY + right._y * i);

            if (line > 0 || i > 0) {
                result.push_back(',');
                if (playerCount > 0 || tile.resourceCount() > 0) {
                    result.push_back(' ');
                }
            }

            appendTileContents(tile, playerCount, result);
        }
    }

    result += "]";
    return result;
}

void handleLook(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    client->getPlayer().addNewEvent(data, 7, client, [&data, client]() {
        if (client->shouldClose()) {
            return;
        }

        client->writeToClient(buildLookReply(data, client));
        client->getPlayer().scheduleNextEvent(data);
    });
}

}
