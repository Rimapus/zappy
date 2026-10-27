/*
** EPITECH PROJECT, 2026
** gui/src/ECS/System/SystemManager
** File description:
** SystemManager header file
*/

#pragma once

#include "../ECSConfig.hpp"
#include "../ECSExceptions.hpp"
#include "../System/System.hpp"
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

/* @brief SystemManager Class */
class SystemManager
{
public:
    /**
     * @brief Register a new system to the manager.
     *
     * @return The pointer to the system.
     */
    template <typename T>
    std::shared_ptr<T> registerSystem(void)
    {
        const char *typeName = typeid(T).name();
        auto system = std::make_shared<T>();

        if (_systems.find(typeName) != _systems.end()) {
            throw SystemAlreadyRegistered(std::string(typeName));
        }
        _systems.insert({typeName, system});
        return system;
    }

    /**
     * @brief Set a signature for a system.
     *
     * @param signature	 Signature to set for a template
     */
    template <typename T>
    void setSignature(Signature signature)
    {
        const char *typeName = typeid(T).name();

        if (_systems.find(typeName) == _systems.end()) {
            throw SystemNotRegistered(std::string(typeName));
        }
        _signatures[typeName] = signature;
    }

    /**
     * @brief Remove an entity in systems
     *
     * @param entity	 Entity to remove
     */
    void entityDestroyed(Entity entity);

    /**
     * @brief Event signature when an entity is changed.
     *
     * @param entity         	 The entity who changed
     * @param entitySignature	 The new entity signature
     */
    void entitySignatureChanged(Entity entity, Signature entitySignature);

private:
    std::unordered_map<const char *, Signature> _signatures{};
    std::unordered_map<const char *, std::shared_ptr<System>> _systems{};
};
