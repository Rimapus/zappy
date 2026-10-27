/*
** EPITECH PROJECT, 2026
** test_Sgt.cpp
** File description:
** Tests for Sgt GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Sgt, returns_frequency)
{
    auto data = test::createTestServerData(500);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleSgt(data, client, "");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sgt 500\n");
    test::closeTestFd(fd);
}

Test(Sgt, args_with_data_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleSgt(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}
