/*
** EPITECH PROJECT, 2026
** gui/src/ECS/System/System
** File description:
** System header file
*/

#pragma once

#include "../ECSConfig.hpp"
#include <set>

/* @brief System Class */
class System
{
public:
    std::set<Entity> _entities;
};
