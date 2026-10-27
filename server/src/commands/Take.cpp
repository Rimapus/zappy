/*
** EPITECH PROJECT, 2026
** Take.cpp
** File description:
** Take command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Player.hpp"
#include "StringUtils.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/resources/IResource.hpp"
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace server {

static bool takeItem(ServerData &data, Player &player, const std::string &str)
{
    auto &tile = data.getMap().at(player.getX(), player.getY());
    auto resource = ResourceType::fromString(str);

    if (resource == ResourceType::COUNT || !tile.hasResource(resource)) {
        return false;
    }
    auto ptr = tile.takeResource(resource);
    ptr->setNatural(false);
    player.getInventory().insert(std::move(ptr));
    return true;
}

void handleTake(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};
    std::string arg;

    if (!(ss >> arg) || !(ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    client->getPlayer().addNewEvent(data, 7, client, [&data, client, arg]() {
        if (client->shouldClose()) {
            return;
        }
        auto &player = client->getPlayer();
        auto normalizedArg = toLowerCase(arg);
        auto resource = ResourceType::fromString(normalizedArg);

        if (takeItem(data, player, normalizedArg)) {
            client->writeToClient("ok");
            data.getGui().broadcast(std::format("pgt #{} {}\n", player.getId(), std::to_underlying(resource)));
            data.getGui().broadcast(data, &handleBct, std::format("{} {}", player.getX(), player.getY()));
            data.getGui().broadcast(data, &handlePin, std::format("{}", player.getId()));
        } else {
            client->writeToClient("ko");
        }

        player.scheduleNextEvent(data);
    });
}

}
