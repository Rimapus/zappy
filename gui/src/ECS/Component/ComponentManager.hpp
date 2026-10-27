/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Component/ComponentManager
** File description:
** ComponentManager header file
*/

#pragma once

#include "../ECSConfig.hpp"
#include "../ECSExceptions.hpp"
#include "Component.hpp"
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

/* @brief ComponentManager */
class ComponentManager
{
public:
    /**
     * @brief Register a component using the template
     */
    template <typename T>
    void registerComponent(void)
    {
        const char *typeName = typeid(T).name();

        if (_componentTypes.find(typeName) != _componentTypes.end()) {
            throw ComponentAlreadyExists(std::string(typeName));
        }
        _componentTypes.insert({typeName, _nextComponentType});
        _componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
        _nextComponentType++;
    }

    /**
     * @brief Get a component type number with the type of the component.
     *
     * @return The component type.
     */
    template <typename T>
    ComponentType getComponentType(void)
    {
        const char *typeName = typeid(T).name();

        if (_componentTypes.find(typeName) == _componentTypes.end()) {
            throw ComponentNotFound(std::string(typeName));
        }
        return _componentTypes[typeName];
    }

    /**
     * @brief Add a component for an entity.
     *
     * @param entity   	 The entity to add
     * @param component	 The component to add to that entity (raw value)
     */
    template <typename T>
    void addComponent(Entity entity, T component)
    {
        getComponentArray<T>()->insertData(entity, component);
    }

    /**
     * @brief Remove a component to an entity.
     *
     * @param entity	 The entity to remove that component
     */
    template <typename T>
    void removeComponent(Entity entity)
    {
        getComponentArray<T>()->removeData(entity);
    }

    /**
     * @brief Get the value of a component of an entity.
     *
     * @param entity	 The entity
     *
     * @return The value of that component for an entity.
     */
    template <typename T>
    T &getComponent(Entity entity)
    {
        return getComponentArray<T>()->getData(entity);
    }

    /**
     * @brief Destroy an entity so we remove his components.
     *
     * @param entity	 Entity to be destroyed
     */
    void entityDestroyed(Entity entity);

private:
    std::unordered_map<const char *, ComponentType> _componentTypes{};
    std::unordered_map<const char *, std::shared_ptr<IComponentArray>> _componentArrays{};
    ComponentType _nextComponentType = 0;

    /**
     * @brief Get pointer to the component array of a T component.
     *
     * @return The shared pointer for the component array.
     */
    template <typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray(void)
    {
        const char *typeName = typeid(T).name();

        if (_componentTypes.find(typeName) == _componentTypes.end()) {
            throw ComponentNotFound(std::string(typeName));
        }
        return std::static_pointer_cast<ComponentArray<T>>(_componentArrays[typeName]);
    }
};
