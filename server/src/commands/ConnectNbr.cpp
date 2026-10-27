/*
** EPITECH PROJECT, 2026
** ConnectNbr.cpp
** File description:
** ConnectNbr command
*/

#include "Client.hpp"
#include "Data.hpp"
#include <algorithm>
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleConnectNbr(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    const auto &teamName = client->getPlayer().getTeam();
    const auto &teams = data.getTeams();

    const auto it = std::ranges::find(teams, teamName);

    if (it == teams.end()) {
        client->writeToClient("0");
        return;
    }

    client->writeToClient(std::format("{}", data.getMap().eggCountForTeam(teamName)));
}

}
