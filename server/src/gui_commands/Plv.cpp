/*
** EPITECH PROJECT, 2026
** Plv.cpp
** File description:
** Plv command
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

namespace server {

void handlePlv(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
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

    client->writeToClient(std::format("plv #{} {}\n", playerNum, player.getLevel()));
}

}
