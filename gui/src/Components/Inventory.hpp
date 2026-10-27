/*
 * EPITECH PROJECT, 2026
 * Inventory.hpp
 * File description:
 * inventory component header
 */

#pragma once

#include <array>
#include <cstdint>

/* @brief Inventory component */
struct Inventory
{
    std::array<std::uint32_t, 7> _resources = {};
};
