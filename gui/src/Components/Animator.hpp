/*
** EPITECH PROJECT, 2026
** gui/src/Components/Animator
** File description:
** Animator component
*/

#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <unordered_map>
#include <vector>

/* @brief AnimationClip Component */
struct AnimationClip
{
    std::vector<sf::IntRect> _frames = {};
    float _frameDuration = 0.0f;
    bool _loop = true;
    std::string _texture = {};
};

/* @brief Animator Component */
struct Animator
{
    std::unordered_map<std::string, AnimationClip> _clips;
    std::string _current;
    std::string _queued;
    int _frameIndex = 0;
    float _elapsed = 0.0f;
    bool _playing = true;
};
