/*
** EPITECH PROJECT, 2026
** gui/src/Components/Background
** File description:
** Background component header file
*/

#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <string>

/* @brief Background Component */
struct Background
{
    std::string _textureKey;
    sf::IntRect _rect;
};
