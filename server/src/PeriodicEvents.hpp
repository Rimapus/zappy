/*
** EPITECH PROJECT, 2026
** PeriodicEvents.hpp
** File description:
** Periodic game events
*/

#pragma once

#include <cstddef>
#include <functional>

namespace server {

class ServerData;

void pushScheduledEvent(ServerData &data, std::function<void()> callback, std::size_t timeLimit);

class PeriodicEventsData
{
public:
    auto &getRefilled(void) const { return _refilled; }

    void setRefilled(bool refilled) { _refilled = refilled; }

    auto &getFoodConsumed(void) const { return _foodConsumed; }

    void setFoodConsumed(bool due) { _foodConsumed = due; }

private:
    bool _refilled{true};
    bool _foodConsumed{true};
};

void processPeriodicEvents(ServerData &data);

}
