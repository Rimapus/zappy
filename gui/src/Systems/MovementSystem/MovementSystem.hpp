/*
** EPITECH PROJECT, 2026
** gui/src/Systems/MovementSystem
** File description:
** MovementSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that smoothly interpolates entity transforms between tiles */
class MovementSystem : public System
{
public:
    /**
     * @brief Advance every in-flight move by dt, writing the eased position
     *        into each entity's Transform before it is rendered.
     *
     * @param coordinator      The coordinator to access components
     * @param dt               Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);
};
