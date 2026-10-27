/*
** EPITECH PROJECT, 2026
** Sibur.hpp
** File description:
** Include file for the Sibur
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Sibur : public AResource
{
public:
    Sibur(void)
        : AResource(ResourceType::SIBUR) {}
};

}
