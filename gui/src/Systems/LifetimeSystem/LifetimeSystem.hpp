/*
** EPITECH PROJECT, 2026
** gui/src/Systems/LifetimeSystem
** File description:
** LifetimeSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that destroys entities once their Lifetime has elapsed */
class LifetimeSystem : public System
{
public:
    /**
     * @brief Tick every entity's lifetime and destroy the expired ones.
     *
     * @param coordinator      The coordinator to access components
     * @param dt               Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);
};
