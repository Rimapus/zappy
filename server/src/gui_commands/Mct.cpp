/*
** EPITECH PROJECT, 2026
** Mct.cpp
** File description:
** Mct command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "gui_commands/GuiCommands.hpp"
#include <cstddef>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleMct(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    auto &map = data.getMap();
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    std::string totalResponse;
    totalResponse.reserve(map.width() * map.height() * 100);

    for (std::size_t y = 0; y < map.height(); y++) {
        for (std::size_t x = 0; x < map.width(); x++) {
            totalResponse += getTileContentString(x, y, map.at(x, y));
        }
    }

    client->writeToClient(totalResponse);
}

}
