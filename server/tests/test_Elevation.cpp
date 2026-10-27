/*
** EPITECH PROJECT, 2026
** test_Elevation.cpp
** File description:
** tests elevation enums
*/

#include "commands/Elevation.hpp"
#include "map/resources/IResource.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Elevation, requirements_size)
{
    cr_assert_eq(ELEVATION_REQUIREMENTS.size(), 7);
}

Test(Elevation, level_1_to_2)
{
    const auto &req = ELEVATION_REQUIREMENTS[0];
    cr_assert_eq(req._players, 1);
    cr_assert_eq(req._resources.size(), 1);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 1);
}

Test(Elevation, level_2_to_3)
{
    const auto &req = ELEVATION_REQUIREMENTS[1];
    cr_assert_eq(req._players, 2);
    cr_assert_eq(req._resources.size(), 3);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 1);
    cr_assert_eq(req._resources[1].first, ResourceType::DERAUMERE);
    cr_assert_eq(req._resources[1].second, 1);
    cr_assert_eq(req._resources[2].first, ResourceType::SIBUR);
    cr_assert_eq(req._resources[2].second, 1);
}

Test(Elevation, level_3_to_4)
{
    const auto &req = ELEVATION_REQUIREMENTS[2];
    cr_assert_eq(req._players, 2);
    cr_assert_eq(req._resources.size(), 2);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 2);
    cr_assert_eq(req._resources[1].first, ResourceType::SIBUR);
    cr_assert_eq(req._resources[1].second, 1);
}

Test(Elevation, level_4_to_5)
{
    const auto &req = ELEVATION_REQUIREMENTS[3];
    cr_assert_eq(req._players, 4);
    cr_assert_eq(req._resources.size(), 4);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 1);
    cr_assert_eq(req._resources[1].first, ResourceType::DERAUMERE);
    cr_assert_eq(req._resources[1].second, 1);
    cr_assert_eq(req._resources[2].first, ResourceType::SIBUR);
    cr_assert_eq(req._resources[2].second, 2);
    cr_assert_eq(req._resources[3].first, ResourceType::PHIRAS);
    cr_assert_eq(req._resources[3].second, 1);
}

Test(Elevation, level_5_to_6)
{
    const auto &req = ELEVATION_REQUIREMENTS[4];
    cr_assert_eq(req._players, 4);
    cr_assert_eq(req._resources.size(), 4);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 1);
    cr_assert_eq(req._resources[1].first, ResourceType::DERAUMERE);
    cr_assert_eq(req._resources[1].second, 2);
    cr_assert_eq(req._resources[2].first, ResourceType::SIBUR);
    cr_assert_eq(req._resources[2].second, 1);
    cr_assert_eq(req._resources[3].first, ResourceType::MENDIANE);
    cr_assert_eq(req._resources[3].second, 3);
}

Test(Elevation, level_6_to_7)
{
    const auto &req = ELEVATION_REQUIREMENTS[5];
    cr_assert_eq(req._players, 6);
    cr_assert_eq(req._resources.size(), 4);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 1);
    cr_assert_eq(req._resources[1].first, ResourceType::DERAUMERE);
    cr_assert_eq(req._resources[1].second, 2);
    cr_assert_eq(req._resources[2].first, ResourceType::SIBUR);
    cr_assert_eq(req._resources[2].second, 3);
    cr_assert_eq(req._resources[3].first, ResourceType::PHIRAS);
    cr_assert_eq(req._resources[3].second, 1);
}

Test(Elevation, level_7_to_8)
{
    const auto &req = ELEVATION_REQUIREMENTS[6];
    cr_assert_eq(req._players, 6);
    cr_assert_eq(req._resources.size(), 6);
    cr_assert_eq(req._resources[0].first, ResourceType::LINEMATE);
    cr_assert_eq(req._resources[0].second, 2);
    cr_assert_eq(req._resources[1].first, ResourceType::DERAUMERE);
    cr_assert_eq(req._resources[1].second, 2);
    cr_assert_eq(req._resources[2].first, ResourceType::SIBUR);
    cr_assert_eq(req._resources[2].second, 2);
    cr_assert_eq(req._resources[3].first, ResourceType::MENDIANE);
    cr_assert_eq(req._resources[3].second, 2);
    cr_assert_eq(req._resources[4].first, ResourceType::PHIRAS);
    cr_assert_eq(req._resources[4].second, 2);
    cr_assert_eq(req._resources[5].first, ResourceType::THYSTAME);
    cr_assert_eq(req._resources[5].second, 1);
}
