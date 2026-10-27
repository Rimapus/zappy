/*
** EPITECH PROJECT, 2026
** test_Inventory.cpp
** File description:
** Tests for Inventory command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Inventory, empty_args_returns_inventory)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleInventory(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]\n");
    test::closeTestFd(fd);
}

Test(Inventory, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleInventory(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}
