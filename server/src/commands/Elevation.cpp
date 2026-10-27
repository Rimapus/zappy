/*
** EPITECH PROJECT, 2026
** Elevation.cpp
** File description:
** Elevation command
*/

#include "Elevation.hpp"
#include "Client.hpp"
#include "Data.hpp"
#include "PeriodicEvents.hpp"
#include "Player.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/Tile.hpp"
#include "map/resources/IResource.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <functional>
#include <istream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace server {

static bool tileHasRequirements(const Tile &tile, const ElevationRequirement &requirement)
{
    for (std::size_t i = 0; i < requirement._resources.size(); i++) {
        const auto &[type, amount] = requirement._resources[i];

        if (!tile.hasResource(type, amount)) {
            return false;
        }
    }

    return true;
}

static void consumeRequirements(Tile &tile, const ElevationRequirement &requirement)
{
    for (std::size_t i = 0; i < requirement._resources.size(); i++) {
        const auto &[type, amount] = requirement._resources[i];
        tile.takeResources(type, amount);
    }
}

static std::vector<std::shared_ptr<Client>> gatherEligiblePlayers(
    ServerData &data,
    const std::shared_ptr<Client> &initiator)
{
    std::vector<std::shared_ptr<Client>> players;
    const auto &initiatorPlayer = initiator->getPlayer();

    for (const auto &client : data.getClientsData()._activeClients) {
        if (!client || client->shouldClose() || client->getPlayer().isFrozen()) {
            continue;
        }
        const auto &player = client->getPlayer();

        if (player.getX() != initiatorPlayer.getX() || player.getY() != initiatorPlayer.getY() || player.getLevel() != initiatorPlayer.getLevel()) {
            continue;
        }

        players.push_back(client);
    }

    return players;
}

static std::vector<std::shared_ptr<Client>> selectBalancedGroup(
    const std::vector<std::shared_ptr<Client>> &players,
    const std::shared_ptr<Client> &initiator,
    std::size_t requiredPlayers)
{
    if (players.size() < requiredPlayers) {
        return {};
    }

    std::map<std::string, std::vector<std::shared_ptr<Client>>> byTeam;

    for (const auto &client : players) {
        byTeam[client->getPlayer().getTeam()].push_back(client);
    }

    std::vector<std::shared_ptr<Client>> selected;
    selected.reserve(requiredPlayers);
    selected.push_back(initiator);

    const auto initiatorTeam = initiator->getPlayer().getTeam();
    auto initiatorIt = byTeam.find(initiatorTeam);
    if (initiatorIt != byTeam.end()) {
        auto &bucket = initiatorIt->second;
        bucket.erase(std::remove(bucket.begin(), bucket.end(), initiator), bucket.end());
    }

    std::map<std::string, std::size_t> pickedPerTeam;
    pickedPerTeam[initiatorTeam] = 1;

    while (selected.size() < requiredPlayers) {
        auto bestIt = byTeam.end();

        for (auto it = byTeam.begin(); it != byTeam.end(); ++it) {
            if (it->second.empty()) {
                continue;
            }
            if (bestIt == byTeam.end()) {
                bestIt = it;
                continue;
            }

            const auto bestCount = pickedPerTeam[bestIt->first];
            const auto currentCount = pickedPerTeam[it->first];

            if (currentCount < bestCount || (currentCount == bestCount && it->second.size() < bestIt->second.size()) || (currentCount == bestCount && it->second.size() == bestIt->second.size() && it->first < bestIt->first)) {
                bestIt = it;
            }
        }

        if (bestIt == byTeam.end()) {
            break;
        }

        selected.push_back(bestIt->second.front());
        bestIt->second.erase(bestIt->second.begin());
        pickedPerTeam[bestIt->first]++;
    }

    if (selected.size() != requiredPlayers) {
        return {};
    }

    return selected;
}

static void freezeGroup(const std::vector<std::shared_ptr<Client>> &players)
{
    for (const auto &client : players) {
        if (client) {
            client->getPlayer().setFrozen(true);
        }
    }
}

static void unfreezeGroupAndResume(ServerData &data, const std::vector<std::shared_ptr<Client>> &players)
{
    for (const auto &client : players) {
        if (!client || client->shouldClose() || !client->getPlayer().isFrozen()) {
            continue;
        }

        auto &player = client->getPlayer();
        player.setFrozen(false);
        player.resumeEvents(data);
    }
}

static void sendGroupMessage(const std::vector<std::shared_ptr<Client>> &players, const std::string &message)
{
    for (const auto &client : players) {
        if (client && !client->shouldClose()) {
            client->writeToClient(message);
        }
    }
}

static void checkFinishedGame(ServerData &data, const std::size_t level)
{
    if (level < 8) {
        return;
    }

    std::unordered_map<std::string, std::size_t> countMaxLevel;

    for (const auto &client : data.getClientsData()._activeClients) {
        const auto &player = client->getPlayer();
        if (player.getLevel() == 8) {
            countMaxLevel[player.getTeam()] += 1;
        }
    }

    std::size_t teamsMaxLevelCount = 0;
    for (const auto &[teamName, count] : countMaxLevel) {
        if (count >= 6 && count > teamsMaxLevelCount) {
            teamsMaxLevelCount = count;
        }
    }

    if (teamsMaxLevelCount == 0) {
        return;
    }

    std::erase_if(countMaxLevel, [teamsMaxLevelCount](const auto &item) {
        return item.second < teamsMaxLevelCount;
    });

    for (const auto &[winningTeam, _] : countMaxLevel) {
        data.getGui().broadcast(std::format("seg {}\n", winningTeam));
    }
    data.setFinished(true);
}

void handleElevation(ServerData &data, std::shared_ptr<Client> &client, const std::string &args)
{
    std::stringstream ss{args};

    if (!(ss >> std::ws).eof()) {
        client->writeToClient("ko");
        return;
    }

    client->getPlayer().addNewEvent(data, 0, client, [&data, client]() {
        if (client->shouldClose()) {
            return;
        }

        auto &player = client->getPlayer();
        const auto level = player.getLevel();

        if (level <= 0 || level > ELEVATION_REQUIREMENTS.size()) {
            client->writeToClient("ko");
            player.scheduleNextEvent(data);
            return;
        }

        const auto &requirement = ELEVATION_REQUIREMENTS.at(level - 1);
        const auto x = player.getX();
        const auto y = player.getY();
        auto &tile = data.getMap().at(x, y);

        if (!tileHasRequirements(tile, requirement)) {
            client->writeToClient("ko");
            player.scheduleNextEvent(data);
            return;
        }

        auto eligiblePlayers = gatherEligiblePlayers(data, client);
        auto selectedPlayers = selectBalancedGroup(eligiblePlayers, client, requirement._players);

        if (selectedPlayers.empty()) {
            client->writeToClient("ko");
            player.scheduleNextEvent(data);
            return;
        }

        freezeGroup(selectedPlayers);
        sendGroupMessage(selectedPlayers, "Elevation underway");

        std::string picMsg = std::format("pic {} {} {}", x, y, level);
        for (const auto &p : selectedPlayers) {
            picMsg += std::format(" #{}", p->getPlayer().getId());
        }
        picMsg += "\n";
        data.getGui().broadcast(picMsg);

        pushScheduledEvent(
            data,
            [&data, client, selectedPlayers, requirement, level, x, y]() {
                auto &player = client->getPlayer();
                auto &tile = data.getMap().at(player.getX(), player.getY());

                if (!tileHasRequirements(tile, requirement)) {
                    data.getGui().broadcast(std::format("pie {} {} 0\n", x, y));
                    sendGroupMessage(selectedPlayers, "ko");
                    unfreezeGroupAndResume(data, selectedPlayers);
                    return;
                }

                for (const auto &participant : selectedPlayers) {
                    if (participant && !participant->shouldClose()) {
                        continue;
                    }
                    data.getGui().broadcast(std::format("pie {} {} 0\n", x, y));
                    sendGroupMessage(selectedPlayers, "ko");
                    unfreezeGroupAndResume(data, selectedPlayers);
                    return;
                }

                consumeRequirements(tile, requirement);
                for (const auto &participant : selectedPlayers) {
                    auto &participantPlayer = participant->getPlayer();
                    participantPlayer.setLevel(level + 1);
                    data.getGui().broadcast(data, &handlePlv, std::format("{}", player.getId()));
                    participant->writeToClient(std::format("Current level: {}", participantPlayer.getLevel()));
                }

                data.getGui().broadcast(std::format("pie {} {} 1\n", x, y));
                data.getGui().broadcast(data, &handleBct, std::format("{} {}", x, y));
                unfreezeGroupAndResume(data, selectedPlayers);
                checkFinishedGame(data, level + 1);
            },
            300);

        player.scheduleNextEvent(data);
    });
}

}
