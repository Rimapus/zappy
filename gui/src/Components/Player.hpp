/*
 * EPITECH PROJECT, 2026
 * Player.hpp
 * File description:
 * player component
 */

#pragma once

#include <cstdint>
#include <string>

/* @brief Player component */
struct Player
{
    std::uint32_t _id = 0;
    std::uint32_t _orientation = 0;
    std::uint32_t _level = 0;
    std::string _team;
};
