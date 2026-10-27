/*
** EPITECH PROJECT, 2026
** Commands.cpp
** File description:
** Server commands
*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace server {

class ServerData;
class Client;

void handleConnectNbr(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleLook(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleForward(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleRight(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleLeft(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleBroadcast(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleElevation(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleInventory(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleTake(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleSet(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleEject(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleFork(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);

using commandHandler = std::function<void(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)>;
const std::unordered_map<std::string, commandHandler> COMMANDS = {
    {"connect_nbr", &handleConnectNbr},
    {"look",        &handleLook      },
    {"forward",     &handleForward   },
    {"right",       &handleRight     },
    {"left",        &handleLeft      },
    {"broadcast",   &handleBroadcast },
    {"incantation", &handleElevation },
    {"inventory",   &handleInventory },
    {"take",        &handleTake      },
    {"set",         &handleSet       },
    {"eject",       &handleEject     },
    {"fork",        &handleFork      },
};

}
