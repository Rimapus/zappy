/*
** EPITECH PROJECT, 2026
** test_Set.cpp
** File description:
** Tests for Set command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Set, empty_args_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleSet(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Set, unknown_resource_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleSet(data, client, "foo");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Set, resource_not_in_inventory_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleSet(data, client, "linemate");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Set, sets_food_from_inventory)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    auto &tile = data.getMap().at(0, 0);

    handleSet(data, client, "food");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    cr_assert_eq(client->getPlayer().getInventory().count(ResourceType::FOOD), 9);
    cr_assert_eq(tile.resourceCount(ResourceType::FOOD, false), 1);
    test::closeTestFd(fd);
}
