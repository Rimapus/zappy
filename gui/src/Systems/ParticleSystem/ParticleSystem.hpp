/*
** EPITECH PROJECT, 2026
** gui/src/Systems/ParticleSystem/ParticleSystem
** File description:
** ParticleSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <SFML/Graphics/Color.hpp>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that updates, draws and destroys particles */
class ParticleSystem : public System
{
public:
    /**
     * @brief Integrate all particle velocities, draw them, and destroy expired ones.
     *
     * @param coordinator The coordinator to access components and resources
     * @param dt Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);

    /**
     * @brief Spawn a burst of teleport particles centered on a world tile.
     *
     * @param coordinator The coordinator to create entities into
     * @param cx World X center of the tile
     * @param cy World Y center of the tile
     * @param arrival true = arrival, false = depop
     * @param color Team color applied to the particles
     */
    static void spawnTeleport(Coordinator &coordinator, float cx, float cy, bool arrival, sf::Color color);

    /**
     * @brief Spawn an expanding ring of particles around a player on expulsion.
     *
     * @param coordinator The coordinator to create entities into
     * @param cx World X center of the expelling player
     * @param cy World Y center of the expelling player
     * @param color Team color applied to the particles
     */
    static void spawnShockwave(Coordinator &coordinator, float cx, float cy, float dirAngle, sf::Color color);
};
