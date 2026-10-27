/*
** EPITECH PROJECT, 2026
** test_Client.cpp
** File description:
** Tests for Client class
*/

#include "Client.hpp"
#include "Data.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace server;

Test(Client, readFromClient_reads_data)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    std::string testData = "Hello World\n";
    write(fd, testData.c_str(), testData.size());

    bool result = client->readFromClient();
    cr_assert_eq(result, true);

    std::string line = client->getLineFromBuffer();
    cr_assert_eq(line, "Hello World\n");
    test::closeTestFd(fd);
}

Test(Client, readFromClient_empty_data_triggers_close)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    close(fd);

    bool result = client->readFromClient();
    cr_assert_eq(result, false);
    cr_assert_eq(client->shouldClose(), true);
}

Test(Client, readFromClient_returns_false_when_shouldClose)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    client->kill();
    bool result = client->readFromClient();
    cr_assert_eq(result, false);
    test::closeTestFd(fd);
}

Test(Client, getLineFromBuffer_returns_empty_when_no_newline)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    write(fd, "no newline here", 16);

    client->readFromClient();
    std::string line = client->getLineFromBuffer();
    cr_assert_eq(line, "");
    test::closeTestFd(fd);
}

Test(Client, getLineFromBuffer_returns_first_line)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    write(fd, "first line\nsecond line\n", 23);

    client->readFromClient();
    std::string line = client->getLineFromBuffer();
    cr_assert_eq(line, "first line\n");

    line = client->getLineFromBuffer();
    cr_assert_eq(line, "second line\n");
    test::closeTestFd(fd);
}

Test(Client, writeToClient_appends_newline)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    client->writeToClient("ok");

    std::string response = test::readAll(fd);
    cr_assert_eq(response, "ok\n");
    test::closeTestFd(fd);
}

Test(Client, writeToClient_keeps_existing_newline)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    client->writeToClient("ok\n");

    std::string response = test::readAll(fd);
    cr_assert_eq(response, "ok\n");
    test::closeTestFd(fd);
}

Test(Client, writeToClient_failure_sets_shouldClose)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);
    close(fd);

    client->writeToClient("test");
    cr_assert_eq(client->shouldClose(), true);
}

Test(Client, close_drops_inventory_on_tile)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);
    test::addToActiveClients(data, client);

    client->getPlayer().setPosition(0, 0);
    client->getPlayer().setTeam("team1");

    auto beforeCount = data.getMap().at(0, 0).resourceCount();

    client->close(data);

    cr_assert_geq(data.getMap().at(0, 0).resourceCount(), beforeCount + 1);
    test::closeTestFd(fd);
}

Test(Client, close_no_player_does_nothing)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);
    test::addToActiveClients(data, client);

    client->close(data);

    test::closeTestFd(fd);
}

Test(Client, kill_sets_shouldClose)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    cr_assert_eq(client->shouldClose(), false);
    client->kill();
    cr_assert_eq(client->shouldClose(), true);
    test::closeTestFd(fd);
}

Test(Client, readFromClient_multiple_lines)
{
    auto [client, fd] = test::createTestClient();
    test::readAll(fd);

    write(fd, "line1\nline2\nline3\n", 19);

    client->readFromClient();
    cr_assert_eq(client->getLineFromBuffer(), "line1\n");
    cr_assert_eq(client->getLineFromBuffer(), "line2\n");
    cr_assert_eq(client->getLineFromBuffer(), "line3\n");
    test::closeTestFd(fd);
}
