/*
** EPITECH PROJECT, 2026
** Fork.cpp
** File description:
** Fork command
*/

#include "Client.hpp"
#include "Data.hpp"
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleFork(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    auto &player = client->getPlayer();

    data.getGui().broadcast(std::format("pfk #{}\n", player.getId()));

    client->getPlayer().addNewEvent(data, 42, client, [&data, client]() {
        if (client->shouldClose()) {
            return;
        }

        auto &player = client->getPlayer();

        auto eggId = data.getMap().spawnEggAt(player.getX(), player.getY(), player.getTeam(), player.getId());
        data.getGui().broadcast(std::format("enw #{} #{} {} {}\n",
                                            eggId, player.getId(), player.getX(), player.getY()));

        client->writeToClient("ok");
        player.scheduleNextEvent(data);
    });
}

}
