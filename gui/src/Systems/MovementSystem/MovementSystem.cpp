/*
** EPITECH PROJECT, 2026
** gui/src/Systems/MovementSystem/MovementSystem
** File description:
** MovementSystem source file
*/

#include "MovementSystem.hpp"
#include "../../Components/Movement.hpp"
#include "../../Components/Transform.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Interpolation.hpp"

/**
 * @brief Advance every in-flight move by dt, writing the eased position
 *        into each entity's Transform before it is rendered.
 *
 * @param coordinator      The coordinator to access components
 * @param dt               Elapsed time since the last frame (seconds)
 */
void MovementSystem::update(Coordinator &coordinator, float dt)
{
    for (Entity const &entity : _entities) {
        Transform &transform = coordinator.getComponent<Transform>(entity);
        Movement &movement = coordinator.getComponent<Movement>(entity);

        stepMove(transform, movement, dt);
    }
}
