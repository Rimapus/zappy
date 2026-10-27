/*
** EPITECH PROJECT, 2026
** test_Msz.cpp
** File description:
** Tests for Msz GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Msz, empty_args_returns_size)
{
    auto data = test::createTestServerData(100000, 15, 20);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleMsz(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "msz 15 20\n");
    test::closeTestFd(fd);
}

Test(Msz, args_with_data_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleMsz(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}
