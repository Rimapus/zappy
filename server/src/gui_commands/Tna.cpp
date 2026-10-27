/*
** EPITECH PROJECT, 2026
** Tna.cpp
** File description:
** tna command
*/

#include "Client.hpp"
#include "Data.hpp"
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleTna(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    for (const auto &team : data.getTeams()) {
        client->writeToClient(std::format("tna {}\n", team));
    }
}

}
