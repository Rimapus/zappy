/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Coordinator/Coordinator
** File description:
** Coordinator header file
*/

#pragma once

#include "../../Components/Transform.hpp"
#include "../Component/ComponentManager.hpp"
#include "../ECSExceptions.hpp"
#include "../Entity/EntityManager.hpp"
#include "../System/SystemManager.hpp"
#include <any>
#include <memory>
#include <typeindex>
#include <unordered_map>

/* @brief Coordinator Class */
class Coordinator
{
public:
    /**
     * @brief Coordinator constructor.
     *
     * @return Coordinator instance.
     */
    Coordinator(void);

    /**
     * @brief Create an entity.
     *
     * @return The new entity.
     */
    Entity createEntity(void);

    /**
     * @brief Destroy an entity.
     *
     * @param entity	 The entity to destroy
     */
    void destroyEntity(Entity entity);

    /**
     * @brief Register the component to the component manager.
     */
    template <typename T>
    void registerComponent(void)
    {
        _componentManager->registerComponent<T>();
    }

    /**
     * @brief Add a component to an entity.
     *
     * @param entity   	 The entity to add the component to
     * @param component	 The raw value of the entity component
     */
    template <typename T>
    void addComponent(Entity entity, T component)
    {
        Signature signature = 0;

        _componentManager->addComponent<T>(entity, component);
        signature = _entityManager->getEntitySignature(entity);
        signature.set(_componentManager->getComponentType<T>(), true);
        _entityManager->setEntitySignature(entity, signature);
        _systemManager->entitySignatureChanged(entity, signature);
    }

    /**
     * @brief Remove a component from an entity.
     *
     * @param entity	 The entity to apply changes
     */
    template <typename T>
    void removeComponent(Entity entity)
    {
        Signature signature = 0;

        _componentManager->removeComponent<T>(entity);
        signature = _entityManager->getEntitySignature(entity);
        signature.set(_componentManager->getComponentType<T>(), false);
        _entityManager->setEntitySignature(entity, signature);
        _systemManager->entitySignatureChanged(entity, signature);
    }

    /**
     * @brief Check whether an entity owns a given component.
     *
     * @param entity	 The entity to test
     *
     * @return True if the entity has the component, false otherwise.
     */
    template <typename T>
    bool hasComponent(Entity entity)
    {
        return _entityManager->getEntitySignature(entity).test(_componentManager->getComponentType<T>());
    }

    /**
     * @brief Get a component of an entity.
     *
     * @param entity	 The entity to get the component from
     *
     * @return A reference to the entity's component.
     */
    template <typename T>
    T &getComponent(Entity entity)
    {
        return _componentManager->getComponent<T>(entity);
    }

    /**
     * @brief Get the type of a component.
     *
     * @return The component type.
     */
    template <typename T>
    ComponentType getComponentType(void)
    {
        return _componentManager->getComponentType<T>();
    }

    /**
     * @brief Register a system to the system manager.
     *
     * @return A shared pointer to the registered system.
     */
    template <typename T>
    std::shared_ptr<T> registerSystem(void)
    {
        return _systemManager->registerSystem<T>();
    }

    /**
     * @brief Set the signature of a system.
     *
     * @param signature	 The signature to set for the system
     */
    template <typename T>
    void setSystemSignature(Signature signature)
    {
        _systemManager->setSignature<T>(signature);
    }

    /**
     * @brief Register a unique global resource (window, clock, socket, ...).
     *
     * A resource is a single shared object, unlike a component which is
     * stored once per entity.
     *
     * @param args	 The arguments forwarded to the resource constructor
     *
     * @return A reference to the newly created resource.
     */
    template <typename T, typename... Args>
    T &addResource(Args &&...args)
    {
        std::type_index typeIndex = std::type_index(typeid(T));

        if (_resources.find(typeIndex) != _resources.end()) {
            throw ResourceAlreadyExists(std::string(typeid(T).name()));
        }

        std::shared_ptr<T> resource = std::make_shared<T>(std::forward<Args>(args)...);
        _resources.insert({typeIndex, resource});
        return *resource;
    }

    /**
     * @brief Get a previously registered global resource.
     *
     * @return A reference to the resource.
     */
    template <typename T>
    T &getResource(void)
    {
        std::type_index typeIndex = std::type_index(typeid(T));
        auto it = _resources.find(typeIndex);

        if (it == _resources.end()) {
            throw ResourceNotFound(std::string(typeid(T).name()));
        }
        return *std::any_cast<std::shared_ptr<T>>(it->second);
    }

private:
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<SystemManager> _systemManager;
    std::unordered_map<std::type_index, std::any> _resources;
};
