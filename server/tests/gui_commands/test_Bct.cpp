/*
** EPITECH PROJECT, 2026
** test_Bct.cpp
** File description:
** Tests for Bct GUI command
*/

#include "gui_commands/GuiCommands.hpp"
#include "test_helpers.hpp"
#include "map/Tile.hpp"
#include "map/resources/Food.hpp"
#include "map/resources/Linemate.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <memory>

using namespace server;

Test(Bct, getTileContentString_empty)
{
    Tile tile;
    auto str = getTileContentString(0, 0, tile);
    cr_assert_eq(str, "bct 0 0 0 0 0 0 0 0 0\n");
}

Test(Bct, getTileContentString_with_food)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    auto str = getTileContentString(5, 3, tile);
    cr_assert_eq(str, "bct 5 3 1 0 0 0 0 0 0\n");
}

Test(Bct, getTileContentString_mixed_resources)
{
    Tile tile;
    tile.addResource(std::make_unique<Food>());
    tile.addResource(std::make_unique<Linemate>());
    tile.addResource(std::make_unique<Food>());
    auto str = getTileContentString(1, 2, tile);
    cr_assert_eq(str, "bct 1 2 2 1 0 0 0 0 0\n");
}

Test(Bct, handleBct_invalid_args)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleBct(data, client, "abc");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Bct, handleBct_out_of_bounds)
{
    auto data = test::createTestServerData(100000, 10, 10);
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleBct(data, client, "10 10");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "sbp\n");
    test::closeTestFd(fd);
}

Test(Bct, handleBct_valid_coordinates)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    handleBct(data, client, "0 0");

    auto out = test::readAll(fd);
    cr_assert_eq(out, "bct 0 0 0 0 0 0 0 0 0\n");
    test::closeTestFd(fd);
}
