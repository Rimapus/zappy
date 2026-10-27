/*
** EPITECH PROJECT, 2026
** gui/src/Systems/TileResourceSystem
** File description:
** TileResourceSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"

class Coordinator;

/* @brief System that draws mineral resource sprites on each game tile */
class TileResourceSystem : public System
{
public:
    /**
     * @brief Draw the appropriate resource sprite frame on every tile entity.
     *
     * @param coordinator The coordinator to access components and resources
     */
    void update(Coordinator &coordinator);
};
