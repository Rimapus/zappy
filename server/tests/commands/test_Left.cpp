/*
** EPITECH PROJECT, 2026
** test_Left.cpp
** File description:
** Tests for Left command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Left, empty_args_turns_west)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::North);

    handleLeft(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::West);
    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    test::closeTestFd(fd);
}

Test(Left, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleLeft(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Left, cycles_through_all_directions)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleLeft(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::West);

    handleLeft(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::South);

    handleLeft(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::East);

    handleLeft(data, client, "");
    test::executeGameEvent(data);
    cr_assert_eq(client->getPlayer().getDirection(), Player::Direction::North);

    test::closeTestFd(fd);
}
