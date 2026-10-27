/*
** EPITECH PROJECT, 2026
** PeriodicEvents.cpp
** File description:
** Periodic game events
*/

#include "PeriodicEvents.hpp"
#include "Client.hpp"
#include "Data.hpp"
#include "GameEvent.hpp"
#include "Logger.hpp"
#include "gui_commands/GuiCommands.hpp"
#include "map/resources/IResource.hpp"
#include <cstddef>
#include <format>
#include <functional>
#include <memory>
#include <utility>

namespace server {

void pushScheduledEvent(ServerData &data, std::function<void()> callback, std::size_t timeLimit)
{
    GameEvent event(std::move(callback), timeLimit);

    event.scheduleTask(data);
    data.getGameEvents().push(event);
}

static void scheduleResourceSpawn(ServerData &data)
{
    auto &periodicEvents = data.getPeriodicEvents();

    pushScheduledEvent(
        data,
        [&data, &periodicEvents]() {
            data.getMap().spawnNaturalResources(data);
            periodicEvents.setRefilled(true);
        },
        20);
}

static void consumeFood(ServerData &data)
{
    for (std::shared_ptr<Client> &client : data.getClientsData()._activeClients) {
        if (client->shouldClose()) {
            continue;
        }
        auto &inventory = client->getPlayer().getInventory();
        auto it = inventory.find(ResourceType::FOOD);

        if (it == inventory.end()) {
            Logger::info("Player #{} starved to death.", client->getPlayer().getId());
            client->kill();
            continue;
        }
        inventory.erase(it);
        data.getGui().broadcast(data, &handlePin, std::format("{}", client->getPlayer().getId()));
    }
}

static void scheduleFoodConsumption(ServerData &data)
{
    auto &periodicEvents = data.getPeriodicEvents();

    pushScheduledEvent(
        data,
        [&data, &periodicEvents]() {
            consumeFood(data);
            periodicEvents.setFoodConsumed(true);
        },
        126);
}

void processPeriodicEvents(ServerData &data)
{
    auto &periodicEvents = data.getPeriodicEvents();

    if (periodicEvents.getRefilled()) {
        scheduleResourceSpawn(data);
        periodicEvents.setRefilled(false);
    }
    if (periodicEvents.getFoodConsumed()) {
        scheduleFoodConsumption(data);
        periodicEvents.setFoodConsumed(false);
    }
}

}
