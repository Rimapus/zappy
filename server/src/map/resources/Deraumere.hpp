/*
** EPITECH PROJECT, 2026
** Deraumere.hpp
** File description:
** Include file for the Deraumere
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Deraumere : public AResource
{
public:
    Deraumere(void)
        : AResource(ResourceType::DERAUMERE) {}
};

}
