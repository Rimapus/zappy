/*
** EPITECH PROJECT, 2026
** Eject.cpp
** File description:
** Eject command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Player.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/Egg.hpp"
#include <array>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <istream>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace server {

static bool destroyEggs(ServerData &data, const Player &player)
{
    auto &eggs = data.getMap().getEggs();
    bool erased = false;

    for (auto it = eggs.begin(); it != eggs.end();) {
        if (it->getX() == player.getX() && it->getY() == player.getY()) {
            data.getGui().broadcast(std::format("edi #{}\n", it->getId()));
            it = eggs.erase(it);
            erased = true;
            continue;
        }
        it++;
    }
    return erased;
}

static std::vector<std::shared_ptr<Client>> gatherEligibleClients(
    ServerData &data,
    const std::shared_ptr<Client> &initClient)
{
    std::vector<std::shared_ptr<Client>> players;
    auto &initiatorPlayer = initClient->getPlayer();

    for (const auto &client : data.getClientsData()._activeClients) {
        if (!client || client->shouldClose() || client == initClient) {
            continue;
        }
        const auto &player = client->getPlayer();

        if (player.getX() != initiatorPlayer.getX() || player.getY() != initiatorPlayer.getY()) {
            continue;
        }

        players.push_back(client);
    }

    return players;
}

static bool ejectPlayers(ServerData &data, const std::shared_ptr<Client> &client)
{
    const std::array<std::array<int, 4>, 4> matrix = {
        {{5, 7, 1, 3},
         {3, 5, 7, 1},
         {1, 3, 5, 7},
         {7, 1, 3, 5}}
    };
    auto &player = client->getPlayer();
    auto eligibleClient = gatherEligibleClients(data, client);
    if (eligibleClient.empty()) {
        return false;
    }
    auto pushDirection = player.getDirection();
    for (auto &client : eligibleClient) {
        auto &player = client->getPlayer();
        auto currentDirection = player.getDirection();

        player.setDirection(pushDirection);
        player.moveForward(data.getMap());
        player.setDirection(currentDirection);

        data.getGui().broadcast(data, &handlePpo, std::format("{}", player.getId()));

        auto row = std::to_underlying(currentDirection);
        auto col = std::to_underlying(pushDirection);

        client->writeToClient(std::format("eject: {}\n", matrix[row][col]));
    }

    return true;
}

void handleEject(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
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
        auto &player = client->getPlayer();

        auto eggsDestroyed = destroyEggs(data, player);
        auto playersEjected = ejectPlayers(data, client);

        if (eggsDestroyed || playersEjected) {
            data.getGui().broadcast(std::format("pex #{}\n", player.getId()));
            client->writeToClient("ok");
        } else {
            client->writeToClient("ko");
        }

        player.scheduleNextEvent(data);
    });
}

}
