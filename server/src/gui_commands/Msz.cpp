/*
** EPITECH PROJECT, 2026
** Msz.cpp
** File description:
** Msz command
*/

#include "Client.hpp"
#include "Data.hpp"
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleMsz(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    auto &map = data.getMap();
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    client->writeToClient(std::format("msz {} {}\n", map.width(), map.height()));
}

}
