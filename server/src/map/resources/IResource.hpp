/*
** EPITECH PROJECT, 2026
** IResource.hpp
** File description:
** Include file for the IResource
*/

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <string_view>

namespace server {

class ResourceType
{
public:
    enum Value {
        FOOD,
        LINEMATE,
        DERAUMERE,
        SIBUR,
        MENDIANE,
        PHIRAS,
        THYSTAME,
        COUNT,
    };

    Value _value;

    constexpr ResourceType(Value v)
        : _value(v) {}

    constexpr std::string_view toString(void) const
    {
        return toString(_value);
    }

    static constexpr std::string_view toString(Value value)
    {
        switch (value) {
        case FOOD:
            return "food";
        case LINEMATE:
            return "linemate";
        case DERAUMERE:
            return "deraumere";
        case SIBUR:
            return "sibur";
        case MENDIANE:
            return "mendiane";
        case PHIRAS:
            return "phiras";
        case THYSTAME:
            return "thystame";
        default:
            return {};
        }
    }

    static constexpr Value fromString(const std::string &string)
    {
        if (string == "food") {
            return FOOD;
        } else if (string == "linemate") {
            return LINEMATE;
        } else if (string == "deraumere") {
            return DERAUMERE;
        } else if (string == "sibur") {
            return SIBUR;
        } else if (string == "mendiane") {
            return MENDIANE;
        } else if (string == "phiras") {
            return PHIRAS;
        } else if (string == "thystame") {
            return THYSTAME;
        }

        return COUNT;
    }

    friend std::ostream &operator<<(std::ostream &os, const ResourceType &r)
    {
        return os << r.toString();
    }

    constexpr bool operator==(const ResourceType &) const = default;
};

class IResource
{
public:
    virtual ~IResource(void) = default;

    virtual ResourceType getType(void) const = 0;

    virtual bool isNatural(void) const = 0;

    virtual void setNatural(bool natural) = 0;
};

inline std::ostream &operator<<(std::ostream &os, const IResource &resource)
{
    return os << resource.getType();
}

struct ResourcePtrCompare
{
    using is_transparent = void;

    bool operator()(const std::unique_ptr<IResource> &leftSide, const std::unique_ptr<IResource> &rightSide) const
    {
        if (!leftSide || !rightSide) {
            return leftSide < rightSide;
        }
        return leftSide->getType()._value < rightSide->getType()._value;
    }

    bool operator()(const std::unique_ptr<IResource> &leftSide, ResourceType::Value rightSide) const
    {
        if (!leftSide) {
            return true;
        }
        return leftSide->getType()._value < rightSide;
    }

    bool operator()(ResourceType::Value leftSide, const std::unique_ptr<IResource> &rightSide) const
    {
        if (!rightSide) {
            return false;
        }
        return leftSide < rightSide->getType()._value;
    }
};

}
