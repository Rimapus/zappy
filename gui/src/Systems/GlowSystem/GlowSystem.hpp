/*
** EPITECH PROJECT, 2026
** gui/src/Systems/GlowSystem
** File description:
** GlowSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that pulses the color of glowing sprites (e.g. incantations) */
class GlowSystem : public System
{
public:
    /**
     * @brief Advance every glow by dt and write the pulsing tint into each
     *        entity's Sprite before it is rendered.
     *
     * @param coordinator      The coordinator to access components
     * @param dt               Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);
};
