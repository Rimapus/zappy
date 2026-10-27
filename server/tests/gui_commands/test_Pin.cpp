/*
** EPITECH PROJECT, 2026
** test_Pin.cpp
** File description:
** Tests for Pin GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Pin, invalid_args_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handlePin(data, client, "abc");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Pin, player_not_found_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handlePin(data, client, "999");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Pin, valid_player_returns_inventory)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(2, 4);

    handlePin(data, client, std::format("{}", client->getPlayer().getId()));

    auto out = test::readAll(fd);
    auto expected = std::format("pin #{} 2 4 10 0 0 0 0 0 0\n",
        client->getPlayer().getId());
    cr_assert_eq(out, expected);
    test::closeTestFd(fd);
}
