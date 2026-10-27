/*
** EPITECH PROJECT, 2026
** Pin.cpp
** File description:
** Pin command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Enum.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/resources/IResource.hpp"
#include <algorithm>
#include <cstddef>
#include <format>
#include <istream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

namespace server {

std::string getInventoryString(const std::multiset<std::unique_ptr<IResource>, ResourcePtrCompare> &inventory)
{
    std::string output = "";
    auto startEnum = ResourceType::FOOD;

    output.reserve(128);
    for (const auto item : EnumRange(startEnum)) {
        output += std::format(" {}", inventory.count(item));
    }
    output += '\n';
    return output;
}

void handlePin(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};
    std::size_t playerNum;
    auto &activeClients = data.getClientsData()._activeClients;

    if (!(ss >> playerNum) || !(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    auto curClient = std::find_if(activeClients.begin(), activeClients.end(), [playerNum](std::shared_ptr<Client> &client) {
        return client->getPlayer().getId() == playerNum;
    });

    if (curClient == activeClients.end()) {
        client->writeToClient("sbp\n");
        return;
    }

    const auto &player = (*curClient)->getPlayer();
    const auto &inventory = player.getInventory();
    std::string inventoryStr = getInventoryString(inventory);

    client->writeToClient(std::format("pin #{} {} {}", playerNum, player.getX(), player.getY()) + inventoryStr);
}

}
