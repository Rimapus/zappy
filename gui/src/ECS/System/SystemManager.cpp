/*
** EPITECH PROJECT, 2026
** gui/src/ECS/System/SystemManager
** File description:
** SystemManager source file
*/

#include "SystemManager.hpp"

/**
 * @brief Remove an entity in systems
 *
 * @param entity	 Entity to remove
 */
void SystemManager::entityDestroyed(Entity entity)
{
    for (auto const &pair : _systems) {
        auto const &system = pair.second;

        system->_entities.erase(entity);
    }
}

/**
 * @brief Event signature when an entity is changed.
 *
 * @param entity         	 The entity who changed
 * @param entitySignature	 The new entity signature
 */
void SystemManager::entitySignatureChanged(Entity entity, Signature entitySignature)
{
    for (auto const &pair : _systems) {
        auto const &type = pair.first;
        auto const &system = pair.second;
        auto const &systemSignature = _signatures[type];

        if ((entitySignature & systemSignature) == systemSignature) {
            system->_entities.insert(entity);
        } else {
            system->_entities.erase(entity);
        }
    }
}
