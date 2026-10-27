/*
 * EPITECH PROJECT, 2026
 * TeamColor.cpp
 * File description:
 * Deterministic per-team tint color
 */

#include "TeamColor.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iterator>

/* Bright, distinct tints so the underlying sprite stays readable once multiplied by sf::Sprite::setColor. */
static const std::array<sf::Color, 8> TEAM_PALETTE = {
    sf::Color(255, 120, 120),
    sf::Color(120, 180, 255),
    sf::Color(140, 230, 140),
    sf::Color(255, 220, 120),
    sf::Color(210, 150, 255),
    sf::Color(120, 235, 235),
    sf::Color(255, 170, 110),
    sf::Color(240, 140, 200)};

sf::Color teamColor(const std::vector<std::string> &teams, const std::string &teamName)
{
    auto it = std::find(teams.begin(), teams.end(), teamName);

    if (it != teams.end()) {
        auto index = static_cast<std::size_t>(std::distance(teams.begin(), it));
        return TEAM_PALETTE[index % TEAM_PALETTE.size()];
    }

    std::size_t hash = std::hash<std::string>{}(teamName);
    auto bright = [](std::size_t value) {
        return static_cast<std::uint8_t>(110 + (value % 146));
    };
    return sf::Color(bright(hash), bright(hash >> 8), bright(hash >> 16));
}
