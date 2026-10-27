/*
** EPITECH PROJECT, 2026
** test_Map_additional.cpp
** File description:
** Additional tests for Map
*/

#include "map/Map.hpp"
#include "Data.hpp"
#include "test_helpers.hpp"
#include "map/resources/Food.hpp"
#include "map/resources/Linemate.hpp"
#include "map/resources/Deraumere.hpp"
#include "map/resources/Sibur.hpp"
#include "map/resources/Mendiane.hpp"
#include "map/resources/Phiras.hpp"
#include "map/resources/Thystame.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <cstddef>
#include <memory>

using namespace server;

Test(Map, spawnNaturalResources_empty_map_does_nothing)
{
    auto data = test::createTestServerData(100, 0, 0);
    data.getMap().spawnNaturalResources(data);
    cr_assert_eq(data.getMap().width(), 0);
    cr_assert_eq(data.getMap().height(), 0);
}

Test(Map, spawnNaturalResources_adds_resources)
{
    auto data = test::createTestServerData(100, 10, 10);
    auto before = data.getMap().countNaturalResources(ResourceType::FOOD);
    data.getMap().spawnNaturalResources(data);
    auto after = data.getMap().countNaturalResources(ResourceType::FOOD);
    cr_assert_geq(after, before);
}

Test(Map, spawnEggForTeam_empty_map_does_nothing)
{
    Map map(0, 0);
    map.spawnEggForTeam("team1");
    cr_assert_eq(map.getEggs().size(), 0);
}

Test(Map, spawnEggForTeam_adds_egg)
{
    Map map(10, 10);
    map.spawnEggForTeam("team1");
    cr_assert_eq(map.getEggs().size(), 1);
    cr_assert_eq(map.getEggs()[0].getTeamName(), "team1");
}

Test(Map, spawnEggs_adds_multiple_eggs)
{
    Map map(10, 10);
    std::vector<std::string> teams = {"team1", "team2"};
    map.spawnEggs(teams, 3);
    cr_assert_eq(map.getEggs().size(), 6);
    cr_assert_eq(map.eggCountForTeam("team1"), 3);
    cr_assert_eq(map.eggCountForTeam("team2"), 3);
}

Test(Map, eggCountAt_multiple_eggs)
{
    Map map(10, 10);
    map.spawnEggAt(5, 5, "team1");
    map.spawnEggAt(5, 5, "team2");
    cr_assert_eq(map.eggCountAt(5, 5), 2);
}

Test(Map, default_constructor_eggs_empty)
{
    Map map;
    cr_assert_eq(map.getEggs().size(), 0);
}
