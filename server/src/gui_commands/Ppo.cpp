/*
** EPITECH PROJECT, 2026
** Ppo.cpp
** File description:
** Ppo command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "gui_commands/GuiCommands.hpp"
#include <algorithm>
#include <cstddef>
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace server {

void handlePpo(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};
    std::size_t playerNum;
    auto &activeClients = data.getClientsData()._activeClients;

    if (!(ss >> playerNum) || !(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    auto curClient = std::find_if(activeClients.begin(), activeClients.end(), [playerNum](std::shared_ptr<Client> &client) {
        return client->getPlayer().getId() == playerNum;
    });

    if (curClient == activeClients.end()) {
        client->writeToClient("sbp\n");
        return;
    }

    const auto &player = (*curClient)->getPlayer();

    client->writeToClient(std::format("ppo #{} {} {} {}\n", playerNum, player.getX(), player.getY(), std::to_underlying(player.getDirection()) + 1));
}

}
