/*
** EPITECH PROJECT, 2026
** AResource.hpp
** File description:
** Include file for the AResource
*/

#pragma once

#include "IResource.hpp"

namespace server {

class AResource : public IResource
{
public:
    ~AResource(void) override = default;

    bool isNatural(void) const override
    {
        return _natural;
    };

    void setNatural(bool natural) override
    {
        _natural = natural;
    };

protected:
    explicit AResource(ResourceType type) : _type(type) {}

    ResourceType getType(void) const override
    {
        return _type;
    };

private:
    ResourceType _type{ResourceType::FOOD};
    bool _natural{true};
};

}
