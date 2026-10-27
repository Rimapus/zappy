/*
** EPITECH PROJECT, 2026
** test_Tile.cpp
** File description:
** Test tile
*/

#include "map/Tile.hpp"
#include "map/resources/Food.hpp"
#include "map/resources/IResource.hpp"
#include "map/resources/Linemate.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <memory>
#include <utility>

using namespace server;

Test(Tile, initially_empty)
{
    Tile tile;
    cr_assert_eq(tile.resourceCount(), 0);
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 0);
    cr_assert_eq(tile.getResources().size(), 0);
    cr_assert_eq(tile.hasResource(ResourceType::FOOD), false);
}

Test(Tile, add_resource_increases_count)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    cr_assert_eq(tile.resourceCount(), 1);
}

Test(Tile, add_multiple_resources)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());
    tile.addResource(std::make_unique<Food>());
    cr_assert_eq(tile.resourceCount(), 3);
}

Test(Tile, resourceCount_by_type)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());
    tile.addResource(std::make_unique<Food>());
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 2);
    cr_assert_eq(tile.resourceCount(ResourceType::LINEMATE, false), 1);
    cr_assert_eq(tile.resourceCount(ResourceType::DERAUMERE, false), 0);
}

Test(Tile, resourceCount_naturalOnly)
{
    Tile tile;
    auto food = std::make_unique<Food>();
    food->setNatural(true);
    tile.addResource(std::move(food));
    auto linemate = std::make_unique<Linemate>();
    linemate->setNatural(false);
    tile.addResource(std::move(linemate));
    auto food2 = std::make_unique<Food>();
    food2->setNatural(false);
    tile.addResource(std::move(food2));
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, true), 1);
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 2);
}

Test(Tile, hasResource_zero_always_true)
{
    Tile tile;
    cr_assert_eq(tile.hasResource(ResourceType::FOOD, 0), true);
}

Test(Tile, hasResource_returns_false_when_missing)
{
    Tile tile;
    cr_assert_eq(tile.hasResource(ResourceType::FOOD), false);
    cr_assert_eq(tile.hasResource(ResourceType::FOOD, 2), false);
}

Test(Tile, hasResource_returns_true_when_present)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    cr_assert_eq(tile.hasResource(ResourceType::FOOD), true);
    cr_assert_eq(tile.hasResource(ResourceType::FOOD, 1), true);
}

Test(Tile, hasResource_exact_count)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Food>());
    cr_assert_eq(tile.hasResource(ResourceType::FOOD, 2), true);
    cr_assert_eq(tile.hasResource(ResourceType::FOOD, 3), false);
}

Test(Tile, takeResource_removes_and_returns)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());
    auto res = tile.takeResource(ResourceType::FOOD);
    cr_assert_not_null(res);
    cr_assert_eq(res->getType(), ResourceType::FOOD);
    cr_assert_eq(tile.resourceCount(), 1);
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 0);
}

Test(Tile, takeResource_returns_null_when_absent)
{
    Tile tile;
    auto res = tile.takeResource(ResourceType::FOOD);
    cr_assert_null(res);
}

Test(Tile, takeResource_takes_first_occurrence)
{
    Tile tile;
    tile.addResource(std::make_unique<Linemate>());
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());
    auto res = tile.takeResource(ResourceType::LINEMATE);
    cr_assert_not_null(res);
    cr_assert_eq(res->getType(), ResourceType::LINEMATE);
    cr_assert_eq(tile.resourceCount(ResourceType::LINEMATE, false), 1);
}

Test(Tile, takeResources_bulk)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Food>());
    auto taken = tile.takeResources(ResourceType::FOOD, 2);
    cr_assert_eq(taken.size(), 2);
    for (const auto &res : taken) {
        cr_assert_eq(res->getType(), ResourceType::FOOD);
    }
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 1);
}

Test(Tile, takeResources_returns_what_is_available)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    auto taken = tile.takeResources(ResourceType::FOOD, 5);
    cr_assert_eq(taken.size(), 1);
    cr_assert_eq(tile.resourceCount(), 0);
}

Test(Tile, getResources_returns_all_types)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());
    tile.addResource(std::make_unique<Food>());
    auto types = tile.getResources();
    cr_assert_eq(types.size(), 3);
    cr_assert_eq(types[0], ResourceType::FOOD);
    cr_assert_eq(types[1], ResourceType::LINEMATE);
    cr_assert_eq(types[2], ResourceType::FOOD);
}

Test(Tile, move_construction)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    Tile other(std::move(tile));
    cr_assert_eq(other.resourceCount(), 1);
}

Test(Tile, move_assignment)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    Tile other;
    other = std::move(tile);
    cr_assert_eq(other.resourceCount(), 1);
}
