/*
** EPITECH PROJECT, 2026
** Linemate.hpp
** File description:
** Include file for the Linemate
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Linemate : public AResource
{
public:
    Linemate(void)
        : AResource(ResourceType::LINEMATE) {}
};

}
