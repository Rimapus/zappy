/*
** EPITECH PROJECT, 2026
** gui/src/Components/Particle
** File description:
** Particle component header file
*/

#pragma once

#include <SFML/Graphics/Color.hpp>

/* @brief Particle Component: visual properties of a short-lived effect particle */
struct Particle
{
    float _radius = 2.f;
    sf::Color _color;
};
