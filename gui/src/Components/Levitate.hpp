/*
** EPITECH PROJECT, 2026
** gui/src/Components/Levitate
** File description:
** Levitate component header file
*/

#pragma once

/* @brief Levitate Component: makes an entity bob up and down during rendering */
struct Levitate
{
    float _amplitude = 2.f;
    float _speed = 3.f;
    float _elapsed = 0.f;
    float _offset = 0.f;
};
