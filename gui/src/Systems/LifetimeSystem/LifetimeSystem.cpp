/*
** EPITECH PROJECT, 2026
** gui/src/Systems/LifetimeSystem/LifetimeSystem
** File description:
** LifetimeSystem source file
*/

#include "LifetimeSystem.hpp"
#include "../../Components/Lifetime.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include <vector>

/**
 * @brief Tick every entity's lifetime and destroy the expired ones.
 *
 * Destruction is deferred to after the loop because destroyEntity mutates
 * the system's entity set we are iterating over.
 *
 * @param coordinator      The coordinator to access components
 * @param dt               Elapsed time since the last frame (seconds)
 */
void LifetimeSystem::update(Coordinator &coordinator, float dt)
{
    std::vector<Entity> expired;

    for (Entity const &entity : _entities) {
        Lifetime &lifetime = coordinator.getComponent<Lifetime>(entity);

        lifetime._remaining -= dt;
        if (lifetime._remaining <= 0.f) {
            expired.push_back(entity);
        }
    }
    for (Entity const &entity : expired) {
        coordinator.destroyEntity(entity);
    }
}
