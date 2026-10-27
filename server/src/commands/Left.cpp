/*
** EPITECH PROJECT, 2026
** Left.cpp
** File description:
** Left command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "gui_commands/GuiCommands.hpp"
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleLeft(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
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

        player.turnLeft();
        data.getGui().broadcast(data, &handlePpo, std::format("{}", player.getId()));
        client->writeToClient("ok");
        player.scheduleNextEvent(data);
    });
}

}
