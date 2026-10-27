/*
** EPITECH PROJECT, 2026
** test_Ppo.cpp
** File description:
** Tests for Ppo GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Ppo, invalid_args_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handlePpo(data, client, "abc");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Ppo, player_not_found_returns_sbp)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handlePpo(data, client, "999");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Ppo, valid_player_returns_position)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(3, 7);
    client->getPlayer().setDirection(Player::Direction::East);

    handlePpo(data, client, std::format("{}", client->getPlayer().getId()));

    auto out = test::readAll(fd);
    cr_assert_eq(out, std::format("ppo #{} 3 7 {}\n",
        client->getPlayer().getId(), std::to_underlying(Player::Direction::East) + 1));
    test::closeTestFd(fd);
}
