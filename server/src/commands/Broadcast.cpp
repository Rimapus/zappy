/*
** EPITECH PROJECT, 2026
** Broadcast.cpp
** File description:
** Broadcast command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Player.hpp"
#include <cmath>
#include <cstddef>
#include <format>
#include <istream>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>

namespace server {

static long long calculateSmallestDelta(std::size_t xEmit, std::size_t xReciv, std::size_t xMax)
{
    auto emit = static_cast<long long>(xEmit);
    auto reciv = static_cast<long long>(xReciv);
    auto max = static_cast<long long>(xMax);

    long long delta = emit - reciv;

    if (delta > (max / 2)) {
        return delta - max;
    } else if (delta < -(max / 2)) {
        return delta + max;
    }

    return delta;
}

static long long getTile(ServerData &data, Player &emitterP, Player &clientP)
{
    auto dx = calculateSmallestDelta(emitterP.getX(), clientP.getX(), data.getMap().width());
    auto dy = calculateSmallestDelta(emitterP.getY(), clientP.getY(), data.getMap().height());

    if (dx == 0 && dy == 0) {
        return 0;
    }

    auto angle = std::atan2(-static_cast<double>(dy), static_cast<double>(dx)) * (180.0 / M_PI);

    double playerAngle = 0.0;

    switch (clientP.getDirection()) {

    case Player::Direction::East:
        playerAngle = 0.0;
        break;
    case Player::Direction::North:
        playerAngle = 90.0;
        break;
    case Player::Direction::West:
        playerAngle = 180.0;
        break;
    case Player::Direction::South:
        playerAngle = 270.0;
        break;
    }

    double angleRel = angle - playerAngle;
    while (angleRel < 0.0) {
        angleRel += 360.0;
    }
    while (angleRel >= 360.0) {
        angleRel -= 360.0;
    }

    int tileNumber = (static_cast<int>(std::floor((angleRel + 22.5) / 45.0)) % 8) + 1;
    return tileNumber;
}

static void broadcast(ServerData &data, const std::shared_ptr<Client> &emitter, std::string msg)
{
    auto &clients = data.getClientsData()._activeClients;
    auto &emitterP = emitter->getPlayer();

    for (const auto &client : clients) {
        if (client == emitter) {
            continue;
        }
        auto &clientP = client->getPlayer();
        auto tile = getTile(data, emitterP, clientP);
        client->writeToClient(std::format("message {}, {}\n", tile, msg));
    }
}

void handleBroadcast(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if ((ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    client->getPlayer().addNewEvent(data, 7, client, [&data, client, args]() {
        if (client->shouldClose()) {
            return;
        }
        auto &player = client->getPlayer();

        broadcast(data, client, args);

        data.getGui().broadcast(std::format("pbc #{} {}\n", player.getId(), args));

        client->writeToClient("ok");
        player.scheduleNextEvent(data);
    });
}

}
