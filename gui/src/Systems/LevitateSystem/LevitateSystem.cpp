/*
** EPITECH PROJECT, 2026
** gui/src/Systems/LevitateSystem/LevitateSystem
** File description:
** LevitateSystem source file
*/

#include "LevitateSystem.hpp"
#include "../../Components/Levitate.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include <cmath>

/**
 * @brief Advance every levitation elapsed time and recompute its Y offset.
 *
 * @param coordinator The coordinator to access components
 * @param dt Elapsed time since the last frame (seconds)
 */
void LevitateSystem::update(Coordinator &coordinator, float dt)
{
    for (Entity const &entity : _entities) {
        Levitate &lev = coordinator.getComponent<Levitate>(entity);

        lev._elapsed += dt;
        lev._offset = std::sin(lev._elapsed * lev._speed) * lev._amplitude;
    }
}
