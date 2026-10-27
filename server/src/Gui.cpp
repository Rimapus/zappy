/*
** EPITECH PROJECT, 2026
** Gui.cpp
** File description:
** GUI clients manager
*/

#include "Gui.hpp"
#include "Client.hpp"
#include "Data.hpp"
#include "StringUtils.hpp"
#include "gui_commands/GuiCommands.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace server {

void Gui::addClient(std::shared_ptr<Client> client)
{
    _clients.push_back(std::move(client));
}

void Gui::broadcast(const std::string &message)
{
    for (auto &client : _clients) {
        client->writeToClient(message);
    }
}

void Gui::broadcast(ServerData &data, const guiCommandHandler &handler, const std::string &args)
{
    for (auto &client : _clients) {
        handler(data, client, args);
    }
}

void handleGuiClients(ServerData &data, std::shared_ptr<Client> &client)
{
    client->readFromClient();
    while (true) {
        auto line = client->getLineFromBuffer();

        if (line.empty()) {
            return;
        }
        line.pop_back();
        if (line.empty()) {
            continue;
        }

        const std::size_t commandEnd = line.find_first_of(" \t");
        const std::string command = toLowerCase(line.substr(0, commandEnd));

        const auto it = GUI_COMMANDS.find(command);
        if (it == GUI_COMMANDS.end()) {
            client->writeToClient("suc\n");
            return;
        }

        const std::size_t argsStart = line.find_first_not_of(" \t", commandEnd);
        const std::string args = argsStart == std::string::npos ? "" : line.substr(argsStart);
        it->second(data, client, args);
    }
}

}
