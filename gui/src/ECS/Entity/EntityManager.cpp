/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Entity/Entity
** File description:
** Entity source file
*/

#include "EntityManager.hpp"
#include "../ECSExceptions.hpp"
#include <string>

/**
 * @brief EntityManager constructor.
 */
EntityManager::EntityManager(void)
{
    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        _availableEntities.push(e);
    }
}

/**
 * @brief Get an entity from the availables ones and remove it.
 *
 * @return The entity just created.
 */
Entity EntityManager::createEntity(void)
{
    Entity e = 0;

    if (_availableEntities.empty() || _livingEntityCount >= MAX_ENTITIES) {
        throw EntityLimitReached();
    }
    e = _availableEntities.front();
    _availableEntities.pop();
    _livingEntityCount++;
    return e;
}

/**
 * @brief Add back the entity to the availables ones and reset his signature.
 *
 * @param entity	 The entity to remove
 */
void EntityManager::destroyEntity(Entity entity)
{
    if (entity >= MAX_ENTITIES) {
        throw InvalidEntity(std::to_string(entity));
    }
    _signatures[entity].reset();
    _availableEntities.push(entity);
    _livingEntityCount--;
}

/**
 * @brief Set the signature to the entity specified.
 *
 * @param entity   	 The entity to get
 * @param signature	 The signature to set
 */
void EntityManager::setEntitySignature(Entity entity, Signature signature)
{
    if (entity >= MAX_ENTITIES) {
        throw InvalidEntity(std::to_string(entity));
    }
    _signatures[entity] = signature;
}

/**
 * @brief Get the signature bitfield of the entity.
 *
 * @param entity	 The entity
 *
 * @return The signature of the entity.
 */
Signature EntityManager::getEntitySignature(Entity entity)
{
    if (entity >= MAX_ENTITIES) {
        throw InvalidEntity(std::to_string(entity));
    }
    return _signatures[entity];
}
