/*
** EPITECH PROJECT, 2026
** Gui.hpp
** File description:
** GUI clients manager
*/

#pragma once

#include "gui_commands/GuiCommands.hpp"
#include <memory>
#include <string>
#include <vector>

namespace server {

class Client;
class ServerData;

class Gui
{
public:
    void addClient(std::shared_ptr<Client> client);

    void broadcast(const std::string &message);

    void broadcast(ServerData &data, const guiCommandHandler &handler, const std::string &args = "");

    auto &getClients(void) { return _clients; }

    auto &getClients(void) const { return _clients; }

private:
    std::vector<std::shared_ptr<Client>> _clients;
};

void handleGuiClients(ServerData &data, std::shared_ptr<Client> &client);

}
