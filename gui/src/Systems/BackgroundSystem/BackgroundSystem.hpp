/*
** EPITECH PROJECT, 2026
** gui/src/Systems/BackgroundSystem
** File description:
** BackgroundSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that tiles a still background texture across the whole window */
class BackgroundSystem : public System
{
public:
    /**
     * @brief Tile every background over the whole window.
     *
     * @param coordinator      The coordinator to access components and resources
     */
    void update(Coordinator &coordinator);
};
