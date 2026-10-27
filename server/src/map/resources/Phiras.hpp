/*
** EPITECH PROJECT, 2026
** Phiras.hpp
** File description:
** Include file for the Phiras
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Phiras : public AResource
{
public:
    Phiras(void)
        : AResource(ResourceType::PHIRAS) {}
};

}
