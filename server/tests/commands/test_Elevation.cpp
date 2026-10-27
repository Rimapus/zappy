/*
** EPITECH PROJECT, 2026
** test_Elevation.cpp
** File description:
** Tests for Elevation command
*/

#include "commands/Commands.hpp"
#include "test_helpers.hpp"
#include "map/Tile.hpp"
#include "map/resources/Linemate.hpp"
#include "map/resources/Deraumere.hpp"
#include "map/resources/Sibur.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <memory>

using namespace server;

Test(Elevation, args_with_data_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleElevation(data, client, "x");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Elevation, insufficient_tile_resources_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    handleElevation(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Elevation, insufficient_players_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);
    client->getPlayer().setLevel(2);

    auto &tile = data.getMap().at(0, 0);
    tile.addResource(std::make_unique<Linemate>());
    tile.addResource(std::make_unique<Deraumere>());
    tile.addResource(std::make_unique<Sibur>());

    handleElevation(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}

Test(Elevation, successful_elevation)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);
    cr_assert_eq(client->getPlayer().getLevel(), 1);

    auto &tile = data.getMap().at(0, 0);
    tile.addResource(std::make_unique<Linemate>());

    handleElevation(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "Elevation underway\n");

    test::executeGameEvent(data);

    out = test::readAll(fd);
    cr_assert_eq(out, "Current level: 2\n");
    cr_assert_eq(client->getPlayer().getLevel(), 2);
    test::closeTestFd(fd);
}

Test(Elevation, resources_removed_before_completion_returns_ko)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);
    client->getPlayer().setPosition(0, 0);

    auto &tile = data.getMap().at(0, 0);
    tile.addResource(std::make_unique<Linemate>());

    handleElevation(data, client, "");
    test::executeGameEvent(data);

    auto out = test::readAll(fd);
    cr_assert_eq(out, "Elevation underway\n");

    tile.takeResources(ResourceType::LINEMATE, 1);

    test::executeGameEvent(data);

    out = test::readAll(fd);
    cr_assert_eq(out, "ko\n");
    test::closeTestFd(fd);
}
