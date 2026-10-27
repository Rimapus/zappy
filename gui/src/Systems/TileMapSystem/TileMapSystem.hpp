/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TileMapSystem
** File description:
** TileMapSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that draws every tile of a tilemap entity */
class TileMapSystem : public System
{
public:
    /**
     * @brief Draw every tile of each tilemap into the window resource.
     *
     * @param coordinator      The coordinator to access components and resources
     */
    void update(Coordinator &coordinator);
};
