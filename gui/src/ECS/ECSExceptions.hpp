/*
** EPITECH PROJECT, 2026
** gui/src/ECS/ECSExceptions
** File description:
** ECS Exception classes header file
*/

#pragma once

#include <stdexcept>
#include <string>

/* @brief Base exception class for all ECS errors */
class ECSError : public std::runtime_error
{
public:
    /**
     * @brief Construct an ECSError exception.
     *
     * @param msg	 Description of the ECS error
     */
    explicit ECSError(const std::string &msg)
        : std::runtime_error(msg) {}
};

/* @brief Exception thrown when a component already exists for an entity */
class ComponentAlreadyExists : public ECSError
{
public:
    /**
     * @brief Construct a ComponentAlreadyExists exception.
     *
     * @param msg	 Description of the duplicate component
     */
    explicit ComponentAlreadyExists(const std::string &msg)
        : ECSError("Component already exists: " + msg) {}
};

/* @brief Exception thrown when a component is not found */
class ComponentNotFound : public ECSError
{
public:
    /**
     * @brief Construct a ComponentNotFound exception.
     *
     * @param msg	 Description of the missing component
     */
    explicit ComponentNotFound(const std::string &msg)
        : ECSError("Component not found: " + msg) {}
};

/* @brief Exception thrown when an entity is not found in a component */
class EntityNotFound : public ECSError
{
public:
    /**
     * @brief Construct an EntityNotFound exception.
     *
     * @param msg	 Description of the missing entity
     */
    explicit EntityNotFound(const std::string &msg)
        : ECSError("Entity not found: " + msg) {}
};

/* @brief Exception thrown when the maximum number of entities is reached */
class EntityLimitReached : public ECSError
{
public:
    /**
     * @brief Construct an EntityLimitReached exception.
     */
    EntityLimitReached(void)
        : ECSError("Maximum number of entities reached") {}
};

/* @brief Exception thrown when an entity ID is out of valid range */
class InvalidEntity : public ECSError
{
public:
    /**
     * @brief Construct an InvalidEntity exception.
     *
     * @param msg	 Description of the invalid entity
     */
    explicit InvalidEntity(const std::string &msg)
        : ECSError("Invalid entity: " + msg) {}
};

/* @brief Exception thrown when a system is already registered */
class SystemAlreadyRegistered : public ECSError
{
public:
    /**
     * @brief Construct a SystemAlreadyRegistered exception.
     *
     * @param msg	 Description of the duplicate system
     */
    explicit SystemAlreadyRegistered(const std::string &msg)
        : ECSError("System already registered: " + msg) {}
};

/* @brief Exception thrown when a system is not registered */
class SystemNotRegistered : public ECSError
{
public:
    /**
     * @brief Construct a SystemNotRegistered exception.
     *
     * @param msg	 Description of the missing system
     */
    explicit SystemNotRegistered(const std::string &msg)
        : ECSError("System not registered: " + msg) {}
};

/* @brief Exception thrown when a resource is already registered */
class ResourceAlreadyExists : public ECSError
{
public:
    /**
     * @brief Construct a ResourceAlreadyExists exception.
     *
     * @param msg	 Description of the duplicate resource
     */
    explicit ResourceAlreadyExists(const std::string &msg)
        : ECSError("Resource already exists: " + msg) {}
};

/* @brief Exception thrown when a resource is not found */
class ResourceNotFound : public ECSError
{
public:
    /**
     * @brief Construct a ResourceNotFound exception.
     *
     * @param msg	 Description of the missing resource
     */
    explicit ResourceNotFound(const std::string &msg)
        : ECSError("Resource not found: " + msg) {}
};
