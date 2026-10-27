/*
** EPITECH PROJECT, 2026
** test_Eject.cpp
** File description:
** Tests for Eject command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Eject, empty_args_no_others_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    handleEject(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Eject, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleEject(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Eject, ejects_other_player)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    auto [other, ofd] = test::createTestClient();
    test::addToActiveClients(data, other);
    other->getPlayer().setPosition(5, 5);

    handleEject(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ok\n");
    auto otherOut = test::readAll(ofd);
    cr_assert(otherOut.find("eject:") != std::string::npos);
    cr_assert(other->getPlayer().getX() != 5 || other->getPlayer().getY() != 5);
    test::closeTestFd(fd);
    test::closeTestFd(ofd);
}
