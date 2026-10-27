/*
** EPITECH PROJECT, 2026
** Egg.hpp
** File description:
** Egg
*/

#pragma once

#include <cstddef>
#include <optional>
#include <string>

namespace server {

class Egg
{
public:
    Egg(std::size_t x, std::size_t y, const std::string &teamName, std::optional<std::size_t> playerId)
        : _id(_nextId++), _x(x), _y(y), _teamName(teamName), _playerId(playerId) {}

    ~Egg() = default;

    auto getId(void) const { return _id; }

    auto getX(void) const { return _x; }

    auto getY(void) const { return _y; }

    const auto &getTeamName(void) const { return _teamName; }

    const auto &getPlayerId(void) const { return _playerId; }

private:
    static inline std::size_t _nextId = 0;
    std::size_t _id;
    std::size_t _x;
    std::size_t _y;
    std::string _teamName;
    std::optional<std::size_t> _playerId;
};

}
