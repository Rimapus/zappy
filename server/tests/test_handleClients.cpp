/*
** EPITECH PROJECT, 2026
** test_handleClients.cpp
** File description:
** Tests for client handling functions
*/

#include "Client.hpp"
#include "Data.hpp"
#include "Gui.hpp"
#include "test_helpers.hpp"
#include "commands/Commands.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace server;

Test(handlePendingClients, graphic_client_sets_gui_flag)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    data.getClientsData()._pendingClients.push_back(client);
    write(fd, "GRAPHIC\n", 8);

    handlePendingClients(data, client);

    cr_assert_eq(client->isGui(), true);
    test::closeTestFd(fd);
}

Test(handlePendingClients, valid_team_adds_player)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    data.getClientsData()._pendingClients.push_back(client);
    data.getMap().spawnEggAt(5, 5, "team1");
    write(fd, "team1\n", 6);

    handlePendingClients(data, client);

    cr_assert_eq(client->hasPlayer(), true);
    cr_assert_eq(client->getPlayer().getTeam(), "team1");
    cr_assert_eq(client->isGui(), false);
    test::closeTestFd(fd);
}

Test(handlePendingClients, unknown_team_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    data.getClientsData()._pendingClients.push_back(client);
    write(fd, "nonexistent\n", 12);

    handlePendingClients(data, client);

    std::string out = test::readAll(fd);
    cr_assert_neq(out.find("ko"), std::string::npos);
    test::closeTestFd(fd);
}

Test(handleActiveClients, unknown_command_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    write(fd, "notacommand\n", 12);
    handleActiveClients(data, client);

    std::string out = test::readAll(fd);
    cr_assert_neq(out.find("ko"), std::string::npos);
    test::closeTestFd(fd);
}

Test(handleActiveClients, forward_command_schedules_event)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(5, 5);

    write(fd, "Forward\n", 8);
    handleActiveClients(data, client);

    cr_assert_eq(data.getGameEvents().size(), 1);

    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(client->getPlayer().getY(), 4);
    cr_assert_neq(out.find("ok"), std::string::npos);
    test::closeTestFd(fd);
}

Test(handleActiveClients, empty_line_is_skipped)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    write(fd, "\n", 1);
    handleActiveClients(data, client);

    test::closeTestFd(fd);
}

Test(handleGuiClients, unknown_command_returns_suc)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    write(fd, "unknown\n", 8);
    handleGuiClients(data, client);

    std::string out = test::readAll(fd);
    cr_assert_eq(out, "suc\n");
    test::closeTestFd(fd);
}

Test(handleGuiClients, msz_command_is_executed)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    write(fd, "msz\n", 4);
    handleGuiClients(data, client);

    std::string out = test::readAll(fd);
    cr_assert_neq(out.find("msz"), std::string::npos);
    test::closeTestFd(fd);
}
