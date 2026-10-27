/*
** EPITECH PROJECT, 2026
** gui/src/Components/Velocity
** File description:
** Velocity component header file
*/

#pragma once

/* @brief Velocity Component: linear motion with optional per-frame slowdown */
struct Velocity
{
    float _vx = 0.f;
    float _vy = 0.f;
    float _slow = 1.f;
};
