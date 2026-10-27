/*
** EPITECH PROJECT, 2026
** test_Tna.cpp
** File description:
** Tests for Tna GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Tna, returns_team_names)
{
    auto data = test::createTestServerData(100000, 10, 10, {"team1", "team2"});
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleTna(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "tna team1\ntna team2\n");
    test::closeTestFd(fd);
}

Test(Tna, args_with_data_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleTna(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}
