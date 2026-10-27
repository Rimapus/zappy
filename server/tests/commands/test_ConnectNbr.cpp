/*
** EPITECH PROJECT, 2026
** test_ConnectNbr.cpp
** File description:
** Tests for ConnectNbr command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(ConnectNbr, empty_args_returns_egg_count)
{
    auto data = test::createTestServerData(100000, 10, 10, {"team1"});
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setTeam("team1");

    handleConnectNbr(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "0\n");
    test::closeTestFd(fd);
}

Test(ConnectNbr, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleConnectNbr(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(ConnectNbr, unknown_team_returns_zero)
{
    auto data = test::createTestServerData(100000, 10, 10, {"team1"});
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setTeam("unknown");

    handleConnectNbr(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "0\n");
    test::closeTestFd(fd);
}

Test(ConnectNbr, with_egg_counts_correctly)
{
    auto data = test::createTestServerData(100000, 10, 10, {"team1"});
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setTeam("team1");

    data.getMap().spawnEggAt(5, 5, "team1", 0);

    handleConnectNbr(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "1\n");
    test::closeTestFd(fd);
}
