/*
** EPITECH PROJECT, 2026
** Player.hpp
** File description:
** Player
*/

#pragma once

#include "GameEvent.hpp"
#include "map/Map.hpp"
#include "map/resources/Food.hpp"
#include "map/resources/IResource.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <string>

namespace server {

class Client;
class ServerData;

class Player
{
public:
    enum class Direction {
        North,
        East,
        South,
        West,
    };

    Player() : _id(_nextId++)
    {
        for (int i = 0; i < 10; i++) {
            _inventory.insert(std::make_unique<Food>());
        }
    };

    ~Player() = default;

    auto getId(void) const { return _id; }

    auto getX(void) const { return _x; }

    auto getY(void) const { return _y; }

    auto getDirection(void) const { return _direction; }

    auto getLevel(void) const { return _level; }

    auto &getTeam(void) { return _team; }

    auto &getTeam(void) const { return _team; }

    void setPosition(std::size_t x, std::size_t y)
    {
        _x = x;
        _y = y;
    }

    void setDirection(Direction direction) { _direction = direction; }

    void setLevel(std::size_t level) { _level = level; }

    void setTeam(const std::string &str) { _team = str; }

    void addNewEvent(
        ServerData &data,
        std::size_t timeLimit,
        const std::shared_ptr<Client> &owner,
        std::function<void()> callback);

    void scheduleNextEvent(ServerData &data);

    void resumeEvents(ServerData &data);

    void pauseEvent(GameEvent event);

    void setFrozen(bool frozen) { _frozen = frozen; }

    auto isFrozen(void) const { return _frozen; }

    void moveForward(const Map &map);

    void turnRight(void);

    void turnLeft(void);

    auto &getInventory(void) { return _inventory; };

    auto &getInventory(void) const { return _inventory; };

private:
    static inline std::size_t _nextId = 0;
    std::size_t _id;
    std::size_t _x{0};
    std::size_t _y{0};
    Direction _direction = Direction::North;
    std::size_t _level{1};
    std::string _team{};
    bool _eventScheduled = false;
    bool _frozen = false;
    std::optional<GameEvent> _pausedEvent;
    std::queue<GameEvent> _pendingEvents;
    std::multiset<std::unique_ptr<IResource>, ResourcePtrCompare> _inventory;
};

}
