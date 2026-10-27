/*
** EPITECH PROJECT, 2026
** test_Plv.cpp
** File description:
** Tests for Plv GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Plv, invalid_args_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handlePlv(data, client, "abc");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Plv, player_not_found_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handlePlv(data, client, "999");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Plv, valid_player_returns_level)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setLevel(3);

    handlePlv(data, client, std::format("{}", client->getPlayer().getId()));

    auto out = test::readAll(fd);
    cr_assert_eq(out, std::format("plv #{} 3\n", client->getPlayer().getId()));
    test::closeTestFd(fd);
}
