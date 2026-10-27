/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Component
** File description:
** Component header file
*/

#pragma once

#include "../ECSConfig.hpp"
#include "../ECSExceptions.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_map>

/* @brief IComponentArray Class */
class IComponentArray
{
public:
    /**
     * @brief Destructor for the IComponentArray class.
     */
    virtual ~IComponentArray(void) = default;

    /**
     * @brief When an entity is removed we broadcast this to every components.
     *
     * @param entity	 The entity destroyed
     */
    virtual void entityDestroyed(Entity entity) = 0;
};

/* @brief ComponentArray Class */
template <typename T>
class ComponentArray : public IComponentArray
{
public:
    /**
     * @brief Insert a data in the component.
     *
     * @param entity   	 What entity should we add to this component
     * @param component	 The component data to add
     */
    void insertData(Entity entity, T component)
    {
        if (_entityToIndexMap.find(entity) != _entityToIndexMap.end()) {
            throw ComponentAlreadyExists("entity " + std::to_string(entity) + " already has this component");
        }

        std::uint32_t newIndex = _size;
        _entityToIndexMap[entity] = newIndex;
        _indexToEntityMap[newIndex] = entity;
        _components[newIndex] = component;
        _size++;
    }

    /**
     * @brief Remove a data in the component.
     *
     * @param entity	 The entity to remove
     */
    void removeData(Entity entity)
    {
        if (_entityToIndexMap.find(entity) == _entityToIndexMap.end()) {
            throw EntityNotFound("entity " + std::to_string(entity) + " not found in component");
        }
        std::uint32_t indexOfRemovedEntity = _entityToIndexMap[entity];
        std::uint32_t indexOfLastElement = _size - 1;
        _components[indexOfRemovedEntity] = _components[indexOfLastElement];

        Entity entityOfLastElement = _indexToEntityMap[indexOfLastElement];
        _entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        _indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        _entityToIndexMap.erase(entity);
        _indexToEntityMap.erase(indexOfLastElement);
        _size--;
    }

    /**
     * @brief Get the data of an entity.
     *
     * @param entity	 The entity to get the data from
     *
     * @return The data from that entity from this component.
     */
    T &getData(Entity entity)
    {
        if (_entityToIndexMap.find(entity) == _entityToIndexMap.end()) {
            throw EntityNotFound("entity " + std::to_string(entity) + " not found in component");
        }
        return _components[_entityToIndexMap[entity]];
    }

    /**
     * @brief When an entity is removed we received this call.
     *
     * @param entity	 The entity destroyed
     */
    void entityDestroyed(Entity entity) override
    {
        if (_entityToIndexMap.find(entity) != _entityToIndexMap.end()) {
            removeData(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> _components;
    std::unordered_map<Entity, std::uint32_t> _entityToIndexMap;
    std::unordered_map<std::uint32_t, Entity> _indexToEntityMap;
    std::uint32_t _size;
};
