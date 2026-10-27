/*
** EPITECH PROJECT, 2026
** Bct.cpp
** File description:
** Bct command
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Enum.hpp"
#include "map/Tile.hpp"
#include "map/resources/IResource.hpp"
#include <cstddef>
#include <format>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

namespace server {

std::string getTileContentString(std::size_t x, std::size_t y, const Tile &tile)
{
    std::string output = std::format("bct {} {}", x, y);
    auto startEnum = ResourceType::FOOD;

    output.reserve(128);
    for (const auto item : EnumRange(startEnum)) {
        output += std::format(" {}", tile.resourceCount(item, false));
    }
    output += "\n";
    return output;
}

void handleBct(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    auto &map = data.getMap();
    std::size_t x = 0;
    std::size_t y = 0;
    std::stringstream ss{args};

    if (!(ss >> x >> y) || !(ss >> std::ws).eof()) {
        client->writeToClient("sbp\n");
        return;
    }

    if (x >= map.width() || y >= map.height()) {
        client->writeToClient("sbp\n");
        return;
    }

    client->writeToClient(getTileContentString(x, y, map.at(x, y)));
}

}
