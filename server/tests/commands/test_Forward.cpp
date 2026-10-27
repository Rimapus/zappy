/*
** EPITECH PROJECT, 2026
** test_Forward.cpp
** File description:
** Tests for Forward command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Forward, empty_args_moves_north)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getX(), 5);
    cr_assert_eq(client->getPlayer().getY(), 4);
    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    test::closeTestFd(fd);
}

Test(Forward, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    handleForward(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    cr_assert_eq(client->getPlayer().getX(), 5);
    cr_assert_eq(client->getPlayer().getY(), 5);
    test::closeTestFd(fd);
}

Test(Forward, wraps_north)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 0);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getY(), 9);
    test::closeTestFd(fd);
}

Test(Forward, moves_east)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);
    client->getPlayer().setDirection(Player::Direction::East);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getX(), 6);
    test::closeTestFd(fd);
}

Test(Forward, wraps_east)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(9, 5);
    client->getPlayer().setDirection(Player::Direction::East);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getX(), 0);
    test::closeTestFd(fd);
}

Test(Forward, moves_south)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);
    client->getPlayer().setDirection(Player::Direction::South);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getY(), 6);
    test::closeTestFd(fd);
}

Test(Forward, wraps_south)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 9);
    client->getPlayer().setDirection(Player::Direction::South);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getY(), 0);
    test::closeTestFd(fd);
}

Test(Forward, moves_west)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);
    client->getPlayer().setDirection(Player::Direction::West);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getX(), 4);
    test::closeTestFd(fd);
}

Test(Forward, wraps_west)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 5);
    client->getPlayer().setDirection(Player::Direction::West);

    handleForward(data, client, "");
    test::executeGameEvent(data);

    cr_assert_eq(client->getPlayer().getX(), 9);
    test::closeTestFd(fd);
}
