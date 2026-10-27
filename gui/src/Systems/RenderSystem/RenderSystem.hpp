/*
** EPITECH PROJECT, 2026
** gui/src/Systems/RenderSystem
** File description:
** RenderSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <climits>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that draws every drawable entity */
class RenderSystem : public System
{
public:
    /**
     * @brief Draw entities whose z is in [zMin, zMax] into the window resource.
     *
     * @param coordinator      The coordinator to access components and resources
     * @param zMin             Inclusive lower z bound (default: draw all)
     * @param zMax             Inclusive upper z bound (default: draw all)
     */
    void update(Coordinator &coordinator, int zMin = INT_MIN, int zMax = INT_MAX);
};
