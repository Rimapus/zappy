/*
 * EPITECH PROJECT, 2026
 * TeamColor.hpp
 * File description:
 * Deterministic per-team tint color
 */

#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>
#include <vector>

/**
 * @brief Pick a stable tint color for a team.
 *
 * Teams already known (present in @p teams) get a distinct color from a fixed
 * palette based on their join order; any other name falls back to a bright
 * color derived from its hash so it stays deterministic across frames.
 *
 * @param teams     The ordered list of known team names
 * @param teamName  The team to colorize
 * @return sf::Color The tint to multiply onto the entity's sprite
 */
sf::Color teamColor(const std::vector<std::string> &teams, const std::string &teamName);
