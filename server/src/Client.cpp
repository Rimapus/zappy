/*
** EPITECH PROJECT, 2026
** Client.cpp
** File description:
** Client
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Logger.hpp"
#include "Player.hpp"
#include "StringUtils.hpp"
#include "commands/Commands.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/Map.hpp"
#include "map/Tile.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <format>
#include <memory>
#include <random>
#include <string>
#include <utility>

namespace server {

bool Client::readFromClient(void)
{
    if (_shouldClose) {
        return false;
    }

    try {
        constexpr int readSize = 4 * 1024;       // reading by chunk of 4 Ko
        constexpr int maxBufferSize = 20 * 1024; // 20 Ko max before flushing the buffer if no \n found;

        if (_buffer.size() < maxBufferSize) {
            std::string freshData = _socket->read(readSize);

            if (freshData.empty()) {
                throw std::exception();
            }
            _buffer += freshData;
        }

        if (_buffer.size() >= maxBufferSize) {
            Logger::error("Client buffer exceeded max limit ({} bytes) without newline, clearing.", maxBufferSize);
            _buffer.clear();
        }
        return true;

    } catch (const std::exception &e) {
        Logger::error("Failed to read from client ({}), closing.", e.what());
        _shouldClose = true;
    }

    return false;
}

std::string Client::getLineFromBuffer(void)
{
    std::size_t newlinePos = _buffer.find('\n');

    if (newlinePos != std::string::npos) {
        std::string line = _buffer.substr(0, newlinePos + 1);
        _buffer.erase(0, newlinePos + 1);
        return line;
    }
    return "";
}

std::size_t Client::writeToClient(const std::string &str)
{
    try {
        if (str.back() != '\n') {
            return _socket->write(str + "\n");
        }
        return _socket->write(str);
    } catch (const std::exception &e) {
        Logger::error("Failed to write to client ({}), closing.", e.what());
        _shouldClose = true;
        return 0;
    }
}

void giveGraphicInfos(ServerData &data, std::shared_ptr<Client> &client)
{
    handleMsz(data, client);
    handleSgt(data, client);
    handleTna(data, client);
    handleMct(data, client);
    for (const auto &clientP : data.getClientsData()._activeClients) {
        handlePpo(data, client, std::format("{}", clientP->getPlayer().getId()));
    }
    for (const auto &egg : data.getMap().getEggs()) {
        const auto &playerId = egg.getPlayerId();
        if (playerId.has_value()) {
            client->writeToClient(std::format("enw #{} #{} {} {}\n", egg.getId(), playerId.value(), egg.getX(), egg.getY()));
        } else {
            client->writeToClient(std::format("enw #{} #{} {} {}\n", egg.getId(), -1, egg.getX(), egg.getY()));
        }
    }
}

void handlePendingClients(ServerData &data, std::shared_ptr<Client> &client)
{
    auto &teams = data.getTeams();

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

        if (line == "GRAPHIC") {
            client->setGui(true);
            Logger::info("GUI client connected.");
            giveGraphicInfos(data, client);
            return;
        }

        auto it = std::ranges::find(teams, line);
        if (it == teams.end()) {
            client->writeToClient(std::format("ko: team \"{}\" does not exist\n", line));
            continue;
        }
        auto &map = data.getMap();
        auto eggCount = map.eggCountForTeam(line);

        if (eggCount <= 0) {
            client->writeToClient(std::format("ko: team \"{}\" is currently full\n", line));
            continue;
        }
        client->initPlayer();
        client->getPlayer().setTeam(line);
        auto &player = client->getPlayer();

        auto [eggId, eggX, eggY] = map.consumeEggForTeam(line);
        player.setPosition(eggX, eggY);
        data.getGui().broadcast(std::format("ebo #{}\n", eggId));

        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<int> dirDist(0, 3);
        player.setDirection(static_cast<Player::Direction>(dirDist(rng)));

        data.getGui().broadcast(std::format("pnw #{} {} {} {} {} {}\n",
                                            player.getId(), player.getX(), player.getY(),
                                            std::to_underlying(player.getDirection()) + 1,
                                            player.getLevel(), player.getTeam()));

        Logger::info("Player #{} joined team \"{}\".", player.getId(), line);
        client->writeToClient(std::format("{}\n{} {}\n", eggCount - 1, map.width(), map.height()));
        return;
    }
}

void handleActiveClients(ServerData &data, std::shared_ptr<Client> &client)
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

        const auto it = COMMANDS.find(command);
        if (it == COMMANDS.end()) {
            client->writeToClient(std::format("ko: command \"{}\" not found\n", command));
            return;
        }

        const std::size_t argsStart = line.find_first_not_of(" \t", commandEnd);
        const std::string args = argsStart == std::string::npos ? "" : line.substr(argsStart);
        Logger::info("Player #{} executes \"{}\".", client->getPlayer().getId(), line);
        it->second(data, client, args);
    }
}

void Client::close(ServerData &data)
{
    if (!_player) {
        return;
    }
    auto &player = *_player;

    Logger::info("Player #{} disconnected.", player.getId());
    player.getTeam().clear();

    Tile &tile = data.getMap().at(player.getX(), player.getY());
    auto &inventory = player.getInventory();
    while (!inventory.empty()) {
        auto resource = inventory.extract(inventory.begin());
        tile.addResource(std::move(resource.value()));
    }

    data.getGui().broadcast(data, &handleBct, std::format("{} {}", player.getX(), player.getY()));
    data.getGui().broadcast(std::format("pdi #{}\n", player.getId()));

    writeToClient("dead");
}

}
