/*
** EPITECH PROJECT, 2026
** gui/src/Systems/LevitateSystem/LevitateSystem
** File description:
** LevitateSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that makes incanting players bob up and down */
class LevitateSystem : public System
{
public:
    /**
     * @brief Advance every levitation elapsed time and recompute its Y offset.
     *
     * @param coordinator The coordinator to access components
     * @param dt Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);
};
