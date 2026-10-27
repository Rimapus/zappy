/*
** EPITECH PROJECT, 2026
** Mendiane.hpp
** File description:
** Include file for the Mendiane
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Mendiane : public AResource
{
public:
    Mendiane(void)
        : AResource(ResourceType::MENDIANE) {}
};

}
