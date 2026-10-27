/*
** EPITECH PROJECT, 2026
** test_Broadcast.cpp
** File description:
** Tests for Broadcast command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Broadcast, empty_args_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    handleBroadcast(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Broadcast, with_message_sends_ok)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    handleBroadcast(data, client, "hello world");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    test::closeTestFd(fd);
}

Test(Broadcast, broadcasts_to_other_player)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    auto [other, ofd] = test::createTestClient();
    test::addToActiveClients(data, other);
    other->getPlayer().setPosition(5, 5);

    handleBroadcast(data, client, "hi");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    auto otherOut = test::readAll(ofd);
    cr_assert_eq(otherOut, "message 0, hi\n");
    test::closeTestFd(fd);
    test::closeTestFd(ofd);
}
