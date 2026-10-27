/*
** EPITECH PROJECT, 2026
** test_Map.cpp
** File description:
** test map
*/

#include "map/Map.hpp"
#include "map/resources/Food.hpp"
#include "map/resources/IResource.hpp"
#include "map/resources/Linemate.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

using namespace server;

Test(Map, constructor_sets_dimensions)
{
    Map map(10, 20);
    cr_assert_eq(map.width(), 10);
    cr_assert_eq(map.height(), 20);
}

Test(Map, default_constructor_zero_dimensions)
{
    Map map;
    cr_assert_eq(map.width(), 0);
    cr_assert_eq(map.height(), 0);
}

Test(Map, at_valid_coordinates)
{
    Map map(5, 5);
    auto &tile = map.at(0, 0);
    cr_assert_eq(tile.resourceCount(), 0);
    cr_assert_eq(&map.at(2, 3), &map.at(2, 3));
}

Test(Map, at_throws_for_x_out_of_range)
{
    Map map(5, 5);
    cr_assert_throw(map.at(5, 0), std::out_of_range);
    cr_assert_throw(map.at(10, 0), std::out_of_range);
}

Test(Map, at_throws_for_y_out_of_range)
{
    Map map(5, 5);
    cr_assert_throw(map.at(0, 5), std::out_of_range);
    cr_assert_throw(map.at(0, 10), std::out_of_range);
}

Test(Map, at_const_overload)
{
    const Map map(3, 3);
    const auto &tile = map.at(1, 1);
    cr_assert_eq(tile.resourceCount(), 0);
}

Test(Map, at_const_throws)
{
    const Map map(3, 3);
    cr_assert_throw(map.at(3, 1), std::out_of_range);
}

Test(Map, tiles_initialized_empty)
{
    Map map(4, 4);
    for (std::size_t y = 0; y < map.height(); y++) {
        for (std::size_t x = 0; x < map.width(); x++) {
            cr_assert_eq(map.at(x, y).resourceCount(), 0);
        }
    }
}

Test(Map, countNaturalResources_empty_map)
{
    Map map(5, 5);
    cr_assert_eq(map.countNaturalResources(ResourceType::FOOD), 0);
    cr_assert_eq(map.countNaturalResources(ResourceType::LINEMATE), 0);
}

Test(Map, countNaturalResources_with_resources)
{
    Map map(5, 5);
    map.at(0, 0).addResource(std::make_unique<Food>());
    map.at(1, 1).addResource(std::make_unique<Food>());
    map.at(2, 2).addResource(std::make_unique<Linemate>());
    auto nonNatural = std::make_unique<Food>();
    nonNatural->setNatural(false);
    map.at(3, 3).addResource(std::move(nonNatural));
    cr_assert_eq(map.countNaturalResources(ResourceType::FOOD), 2);
    cr_assert_eq(map.countNaturalResources(ResourceType::LINEMATE), 1);
}

Test(Map, spawnEggAt_returns_eggs)
{
    Map map(5, 5);
    auto id = map.spawnEggAt(1, 2, "team1");
    cr_assert_eq(map.getEggs().size(), 1);
    cr_assert_eq(map.getEggs()[0].getId(), id);
    cr_assert_eq(map.getEggs()[0].getX(), 1);
    cr_assert_eq(map.getEggs()[0].getY(), 2);
    cr_assert_eq(map.getEggs()[0].getTeamName(), "team1");
}

Test(Map, spawnEggAt_with_playerId)
{
    Map map(5, 5);
    map.spawnEggAt(3, 4, "team2", 42);
    cr_assert_eq(map.getEggs()[0].getPlayerId().value(), 42);
}

Test(Map, eggCountAt)
{
    Map map(10, 10);
    map.spawnEggAt(0, 0, "team1");
    map.spawnEggAt(0, 0, "team2");
    map.spawnEggAt(1, 1, "team1");
    cr_assert_eq(map.eggCountAt(0, 0), 2);
    cr_assert_eq(map.eggCountAt(1, 1), 1);
    cr_assert_eq(map.eggCountAt(5, 5), 0);
}

Test(Map, eggCountForTeam)
{
    Map map(10, 10);
    map.spawnEggAt(0, 0, "team1");
    map.spawnEggAt(1, 1, "team2");
    map.spawnEggAt(2, 2, "team1");
    cr_assert_eq(map.eggCountForTeam("team1"), 2);
    cr_assert_eq(map.eggCountForTeam("team2"), 1);
    cr_assert_eq(map.eggCountForTeam("nonexistent"), 0);
}

Test(Map, consumeEggForTeam_removes_egg)
{
    Map map(10, 10);
    auto id = map.spawnEggAt(5, 6, "test_team");
    auto result = map.consumeEggForTeam("test_team");
    cr_assert_eq(std::get<0>(result), id);
    cr_assert_eq(std::get<1>(result), 5);
    cr_assert_eq(std::get<2>(result), 6);
    cr_assert_eq(map.eggCountForTeam("test_team"), 0);
    cr_assert_eq(map.getEggs().size(), 0);
}

Test(Map, consumeEggForTeam_empty_returns_zeros)
{
    Map map(10, 10);
    auto result = map.consumeEggForTeam("no_eggs");
    cr_assert_eq(std::get<0>(result), 0);
    cr_assert_eq(std::get<1>(result), 0);
    cr_assert_eq(std::get<2>(result), 0);
}

Test(Map, move_construction)
{
    Map map(3, 4);
    map.spawnEggAt(0, 0, "t");
    Map other(std::move(map));
    cr_assert_eq(other.width(), 3);
    cr_assert_eq(other.height(), 4);
    cr_assert_eq(other.getEggs().size(), 1);
}

Test(Map, move_assignment)
{
    Map map(3, 4);
    Map other;
    other = std::move(map);
    cr_assert_eq(other.width(), 3);
    cr_assert_eq(other.height(), 4);
}
