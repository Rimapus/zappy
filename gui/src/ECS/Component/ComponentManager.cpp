/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Component/ComponentManager
** File description:
** ComponentManager source file
*/

#include "ComponentManager.hpp"

/**
 * @brief Destroy an entity so we remove his components.
 *
 * @param entity	 Entity to be destroyed
 */
void ComponentManager::entityDestroyed(Entity entity)
{
    for (auto const &pair : _componentArrays) {
        auto const &component = pair.second;

        component->entityDestroyed(entity);
    }
}
