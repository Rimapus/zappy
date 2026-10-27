/*
** EPITECH PROJECT, 2026
** Inventory.cpp
** File description:
** Inventory command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Enum.hpp"
#include "Player.hpp"
#include "map/resources/IResource.hpp"
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

static std::string createInventoryString(Player &player)
{
    std::string output = "[";
    auto startEnum = ResourceType::FOOD;

    for (const auto item : EnumRange(startEnum)) {
        if (item != startEnum) {
            output += ", ";
        }
        output += std::format("{} {}", ResourceType::toString(item), player.getInventory().count(item));
    }
    output += "]";
    return output;
}

void handleInventory(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    client->getPlayer().addNewEvent(data, 1, client, [&data, client]() {
        if (client->shouldClose()) {
            return;
        }
        auto &player = client->getPlayer();

        client->writeToClient(createInventoryString(player));

        player.scheduleNextEvent(data);
    });
}

}
