/*
** EPITECH PROJECT, 2026
** Food.hpp
** File description:
** Include file for the Food
*/

#pragma once

#include "AResource.hpp"
#include "map/resources/IResource.hpp"

namespace server {

class Food : public AResource
{
public:
    Food(void)
        : AResource(ResourceType::FOOD) {}
};

}
