/*
** EPITECH PROJECT, 2026
** test_Sst.cpp
** File description:
** Tests for Sst GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Sst, zero_newTick_returns_sbp)
{
    auto data = test::createTestServerData(100);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleSst(data, client, "0");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    cr_assert_eq(data.getFrequency(), 100);
    test::closeTestFd(fd);
}

Test(Sst, valid_newTick_updates_frequency)
{
    auto data = test::createTestServerData(100);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleSst(data, client, "200");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sst 200\n");
    cr_assert_eq(data.getFrequency(), 200);
    test::closeTestFd(fd);
}

Test(Sst, args_with_data_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleSst(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}
