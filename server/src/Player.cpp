/*
** EPITECH PROJECT, 2026
** Player.cpp
** File description:
** Player
*/

#include "Player.hpp"
#include "Data.hpp"
#include "GameEvent.hpp"
#include "map/Map.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

namespace server {

void Player::addNewEvent(
    ServerData &data,
    std::size_t timeLimit,
    const std::shared_ptr<Client> &owner,
    std::function<void()> callback)
{
    if (_pendingEvents.size() >= 9) {
        return;
    }

    GameEvent event(std::move(callback), timeLimit);
    event.setOwner(owner);
    _pendingEvents.push(std::move(event));

    if (!_eventScheduled && !_frozen) {
        _eventScheduled = true;
        scheduleNextEvent(data);
    }
}

void Player::scheduleNextEvent(ServerData &data)
{
    if (_frozen) {
        _eventScheduled = false;
        return;
    }

    if (_pausedEvent.has_value()) {
        _pausedEvent->scheduleTask(data);
        data.getGameEvents().push(*_pausedEvent);
        _pausedEvent.reset();
        return;
    }

    if (_pendingEvents.empty()) {
        _eventScheduled = false;
        return;
    }

    auto event = _pendingEvents.front();
    _pendingEvents.pop();

    event.scheduleTask(data);

    data.getGameEvents().push(event);
}

void Player::resumeEvents(ServerData &data)
{
    if (_frozen || _eventScheduled) {
        return;
    }

    if (_pausedEvent.has_value() || !_pendingEvents.empty()) {
        _eventScheduled = true;
        scheduleNextEvent(data);
    }
}

void Player::pauseEvent(GameEvent event)
{
    _pausedEvent.emplace(std::move(event));
    _eventScheduled = false;
}

void Player::moveForward(const Map &map)
{
    if (map.width() == 0 || map.height() == 0) {
        return;
    }

    switch (_direction) {
    case Direction::North:
        _y = _y == 0 ? map.height() - 1 : _y - 1;
        break;
    case Direction::East:
        _x = (_x + 1) % map.width();
        break;
    case Direction::South:
        _y = (_y + 1) % map.height();
        break;
    case Direction::West:
        _x = _x == 0 ? map.width() - 1 : _x - 1;
        break;
    }
}

void Player::turnRight(void)
{
    _direction = static_cast<Direction>((std::to_underlying(_direction) + 1) % 4);
}

void Player::turnLeft(void)
{
    _direction = static_cast<Direction>((std::to_underlying(_direction) + 3) % 4);
}

}
