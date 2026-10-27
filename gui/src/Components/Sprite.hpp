/*
** EPITECH PROJECT, 2026
** gui/src/Components/Sprite
** File description:
** Sprite component header file
*/

#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <optional>

/* @brief Sprite Component (points to a texture owned elsewhere) */
struct Sprite
{
    std::optional<sf::Sprite> _sprite;
};
