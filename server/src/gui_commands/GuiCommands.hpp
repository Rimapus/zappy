/*
** EPITECH PROJECT, 2026
** GuiCommands.hpp
** File description:
** GUI commands
*/

#pragma once

#include "map/Tile.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace server {

class ServerData;
class Client;

void handleTna(ServerData &data, std::shared_ptr<Client> &client, const std::string &args = "");
void handleMsz(ServerData &data, std::shared_ptr<Client> &client, const std::string &args = "");
void handleMct(ServerData &data, std::shared_ptr<Client> &client, const std::string &args = "");
void handleSgt(ServerData &data, std::shared_ptr<Client> &client, const std::string &args = "");
void handlePpo(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handlePlv(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handlePin(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleBct(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);
void handleSst(ServerData &data, std::shared_ptr<Client> &client, const std::string &args);

std::string getTileContentString(std::size_t x, std::size_t y, const Tile &tile);

using guiCommandHandler = std::function<void(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)>;
const std::unordered_map<std::string, guiCommandHandler> GUI_COMMANDS = {
    {"tna", &handleTna},
    {"msz", &handleMsz},
    {"bct", &handleBct},
    {"mct", &handleMct},
    {"ppo", &handlePpo},
    {"plv", &handlePlv},
    {"pin", &handlePin},
    {"sgt", &handleSgt},
    {"sst", &handleSst},
};

}
