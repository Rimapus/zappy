/*
** EPITECH PROJECT, 2026
** test_Take.cpp
** File description:
** Tests for Take command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include "map/resources/Food.hpp"
#include "map/resources/Linemate.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <memory>

using namespace server;

Test(Take, empty_args_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleTake(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Take, unknown_resource_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleTake(data, client, "foo");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Take, missing_resource_on_tile_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleTake(data, client, "linemate");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Take, takes_food_from_tile)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    auto &tile = data.getMap().at(0, 0);
    tile.addResource(std::make_unique<Food>());

    handleTake(data, client, "food");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 0);
    cr_assert_eq(client->getPlayer().getInventory().count(ResourceType::FOOD), 11);
    test::closeTestFd(fd);
}

Test(Take, takes_multiple_from_tile)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    auto &tile = data.getMap().at(0, 0);
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());

    handleTake(data, client, "linemate");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    cr_assert_eq(tile.resourceCount(ResourceType::LINEMATE, false), 0);
    cr_assert_eq(client->getPlayer().getInventory().count(ResourceType::LINEMATE), 1);
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 1);
    test::closeTestFd(fd);
}
