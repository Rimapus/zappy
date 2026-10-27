/*
** EPITECH PROJECT, 2026
** Thystame.hpp
** File description:
** Include file for the Thystame
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Thystame : public AResource
{
public:
    Thystame(void)
        : AResource(ResourceType::THYSTAME) {}
};

}
