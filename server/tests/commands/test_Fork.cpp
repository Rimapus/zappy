/*
** EPITECH PROJECT, 2026
** test_Fork.cpp
** File description:
** Tests for Fork command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Fork, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleFork(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Fork, spawns_egg)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(3, 7);
    client->getPlayer().setTeam("team1");

    handleFork(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    auto &eggs = data.getMap().getEggs();
    cr_assert_eq(eggs.size(), 1);
    cr_assert_eq(eggs[0].getX(), 3);
    cr_assert_eq(eggs[0].getY(), 7);
    cr_assert_eq(eggs[0].getTeamName(), "team1");
    test::closeTestFd(fd);
}
