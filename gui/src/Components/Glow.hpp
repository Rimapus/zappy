/*
** EPITECH PROJECT, 2026
** gui/src/Components/Glow
** File description:
** Glow component header file
*/

#pragma once

#include <SFML/Graphics/Color.hpp>

/* @brief Glow Component: pulses an entity's sprite color between two tints */
struct Glow
{
    sf::Color _base = sf::Color::White;
    sf::Color _peak = sf::Color::White;
    float _speed = 0.0f;
    float _elapsed = 0.0f;
};
