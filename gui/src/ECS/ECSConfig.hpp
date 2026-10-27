/*
** EPITECH PROJECT, 2026
** gui/src/ECS/ECSConfig
** File description:
** ECS Config header file
*/

#pragma once

#include <bitset>
#include <cstdint>

/* @brief Entity Alias */
using Entity = std::uint32_t;

/* @brief ComponentType Alias */
using ComponentType = std::uint8_t;

/* @brief Max components available in the game */
constexpr ComponentType MAX_COMPONENTS = 32;

/* @brief Signature Alias */
using Signature = std::bitset<MAX_COMPONENTS>;

/* @brief Max of entities availables in the game */
constexpr Entity MAX_ENTITIES = 100000;
