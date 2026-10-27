/*
** EPITECH PROJECT, 2026
** test_Look.cpp
** File description:
** Tests for Look command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Look, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleLook(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Look, sees_self_on_empty_tile)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    handleLook(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert(out.starts_with("[") && out.ends_with("]\n"));
    test::closeTestFd(fd);
}

Test(Look, sees_other_player_next_to)
{
    auto data = test::createTestServerData(100000, 3, 3);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(1, 1);

    auto [other, ofd] = test::createTestClient();
    test::addToActiveClients(data, other);
    other->getPlayer().setPosition(1, 0);

    handleLook(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert(out.find("player") != std::string::npos);
    test::closeTestFd(fd);
    test::closeTestFd(ofd);
}
