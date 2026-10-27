/*
** EPITECH PROJECT, 2026
** test_Mct.cpp
** File description:
** Tests for Mct GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Mct, returns_all_tile_contents)
{
    auto data = test::createTestServerData(100000, 2, 2);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleMct(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out,
        "bct 0 0 0 0 0 0 0 0 0\nbct 1 0 0 0 0 0 0 0 0\n"
        "bct 0 1 0 0 0 0 0 0 0\nbct 1 1 0 0 0 0 0 0 0\n");
    test::closeTestFd(fd);
}

Test(Mct, args_with_data_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleMct(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}
