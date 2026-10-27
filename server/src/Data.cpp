/*
** EPITECH PROJECT, 2026
** Data.cpp
** File description:
** Data
*/

#include "Data.hpp"
#include "GameEvent.hpp"
#include <chrono>
#include <cstddef>
#include <vector>

namespace server {

void GameEvent::scheduleTask(ServerData &data, std::chrono::steady_clock::time_point from)
{
    auto f = data.getFrequency();

    std::chrono::duration<double> offset(static_cast<double>(_timeLimit) / f);

    _executionTime = std::chrono::time_point_cast<std::chrono::steady_clock::duration>(from + offset);
    _scheduledAt = from;
}

void GameEvent::rescheduleTask(ServerData &data)
{
    if (_scheduledAt != std::chrono::steady_clock::time_point{}) {
        scheduleTask(data, _scheduledAt);
    }
}

void ServerData::setFrequency(std::size_t newF)
{
    _frequency = newF;
    std::vector<GameEvent> tempEvents;

    while (!_gameEvents.empty()) {
        tempEvents.push_back(_gameEvents.top());
        _gameEvents.pop();
    }

    for (auto &event : tempEvents) {
        event.rescheduleTask(*this);
    }

    for (const auto &event : tempEvents) {
        _gameEvents.push(event);
    }
}

}
