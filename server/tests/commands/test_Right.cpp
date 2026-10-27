/*
** EPITECH PROJECT, 2026
** test_Right.cpp
** File description:
** Tests for Right command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Right, empty_args_turns_east)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::North);

    handleRight(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::East);
    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    test::closeTestFd(fd);
}

Test(Right, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleRight(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Right, cycles_through_all_directions)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleRight(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::East);

    handleRight(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::South);

    handleRight(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::West);

    handleRight(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::North);

    test::closeTestFd(fd);
}
