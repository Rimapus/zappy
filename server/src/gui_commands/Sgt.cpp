/*
** EPITECH PROJECT, 2026
** Sgt.cpp
** File description:
** Sgt command
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

void handleSgt(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    client->writeToClient(std::format("sgt {}\n", data.getFrequency()));
}

}
