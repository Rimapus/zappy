/*
** EPITECH PROJECT, 2026
** Set.cpp
** File description:
** Set command
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

static bool setItem(ServerData &data, Player &player, const std::string &str)
{
    auto &tile = data.getMap().at(player.getX(), player.getY());
    auto &inventory = player.getInventory();
    auto resource = ResourceType::fromString(str);
    auto it = inventory.find(resource);

    if (it == inventory.end()) {
        return false;
    }
    auto extractedResource = inventory.extract(it);
    extractedResource.value()->setNatural(false);
    tile.addResource(std::move(extractedResource.value()));
    return true;
}

void handleSet(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
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

        if (setItem(data, player, normalizedArg)) {
            client->writeToClient("ok");
            data.getGui().broadcast(std::format("pdr #{} {}\n", player.getId(), std::to_underlying(resource)));
            data.getGui().broadcast(data, &handleBct, std::format("{} {}", player.getX(), player.getY()));
            data.getGui().broadcast(data, &handlePin, std::format("{}", player.getId()));
        } else {
            client->writeToClient("ko");
        }

        player.scheduleNextEvent(data);
    });
}

}
