/*
** EPITECH PROJECT, 2026
** gui/src/Systems/AnimatorSystem
** File description:
** AnimatorSystem header file
*/

#pragma once

#include "../../ECS/System/System.hpp"
#include <string>

/* @brief FWD Declaration of the Coordinator Class */
class Coordinator;

/* @brief System that ticks sprite animations */
class AnimatorSystem : public System
{
public:
    /**
     * @brief Advance every entity's animation by dt and push the current
     *        frame into its Sprite.
     *
     * @param coordinator      The coordinator to access components
     * @param dt               Elapsed time since the last frame (seconds)
     */
    void update(Coordinator &coordinator, float dt);

    /**
     * @brief Play one animation (loops if the clip's _loop is true).
     *
     * Writes the intent into the entity's Animator component; the actual
     * frame stepping happens in update().
     *
     * @param coordinator      The coordinator to access components
     * @param entity           The entity to play the animation on
     * @param name             The name of the animation
     */
    static void play(Coordinator &coordinator, Entity entity, const std::string &name);

    /**
     * @brief Play one animation once, then switch to a looping one.
     *
     * @param coordinator      The coordinator to access components
     * @param entity           The entity to play the animation on
     * @param name             Animation played a single time (clip must be _loop = false)
     * @param loop             Animation looped once `name` has finished
     */
    static void playOnce(Coordinator &coordinator, Entity entity, const std::string &name, const std::string &loop);
};
