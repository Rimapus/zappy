/*
** EPITECH PROJECT, 2026
** GameEvent.hpp
** File description:
** GameEvent
*/

#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>

namespace server {

class Client;
class ServerData;

class GameEvent
{
public:
    std::function<void()> _callback{};
    std::size_t _timeLimit{};
    std::chrono::steady_clock::time_point _executionTime{};
    std::chrono::steady_clock::time_point _scheduledAt{};
    std::weak_ptr<Client> _owner{};

    GameEvent(std::function<void()> callback, std::size_t timeLimit)
        : _callback(callback), _timeLimit(timeLimit) {};

    void scheduleTask(ServerData &data, std::chrono::steady_clock::time_point from = std::chrono::steady_clock::now());

    void rescheduleTask(ServerData &data);

    void setOwner(const std::shared_ptr<Client> &owner) { _owner = owner; }

    auto getOwner(void) const { return _owner.lock(); }
};

struct CompareEventTime
{
    bool operator()(const GameEvent &a, const GameEvent &b)
    {
        return a._executionTime > b._executionTime;
    }
};

}
