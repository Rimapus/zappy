/*
** EPITECH PROJECT, 2026
** Sst.cpp
** File description:
** Sst command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "gui_commands/GuiCommands.hpp"
#include <cstddef>
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

void handleSst(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};
    std::size_t newTick;

    if (!(ss >> newTick) || !(ss >> std::ws).eof() || !newTick) {
        client->writeToClient("sbp\n");
        return;
    }

    data.setFrequency(newTick);

    client->writeToClient(std::format("sst {}\n", newTick));
}

}
