/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Entity
** File description:
** Entity header file
*/

#pragma once

#include "../ECSConfig.hpp"
#include <array>
#include <cstdint>
#include <queue>

/* @brief EntityManager Class */
class EntityManager
{
public:
    /**
     * @brief EntityManager constructor.
     */
    EntityManager(void);

    /**
     * @brief EntityManager destructor.
     */
    ~EntityManager(void) = default;

    /**
     * @brief Get an entity from the availables ones and remove it.
     *
     * @return The entity just created.
     */
    Entity createEntity(void);

    /**
     * @brief Add back the entity to the availables ones and reset his signature.
     *
     * @param entity	 The entity to remove
     */
    void destroyEntity(Entity entity);

    /**
     * @brief Set the signature to the entity specified.
     *
     * @param entity   	 The entity to get
     * @param signature	 The signature to set
     */
    void setEntitySignature(Entity entity, Signature signature);

    /**
     * @brief Get the signature bitfield of the entity.
     *
     * @param entity	 The entity
     *
     * @return The signature of the entity.
     */
    Signature getEntitySignature(Entity entity);

private:
    std::queue<Entity> _availableEntities{};
    std::array<Signature, MAX_ENTITIES> _signatures{};
    uint32_t _livingEntityCount = 0;
};
