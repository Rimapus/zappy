/*
** EPITECH PROJECT, 2026
** test_Player.cpp
** File description:
** test player
*/

#include "Player.hpp"
#include "map/Map.hpp"
#include "map/resources/IResource.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <cstddef>

using namespace server;

Test(Player, default_constructor)
{
    Player player;
    cr_assert_eq(player.getLevel(), 1);
    cr_assert_eq(player.getDirection(), Player::Direction::North);
    cr_assert_eq(player.getX(), 0);
    cr_assert_eq(player.getY(), 0);
    cr_assert_eq(player.getTeam(), "");
    cr_assert_eq(player.isFrozen(), false);
}

Test(Player, default_inventory_has_ten_food)
{
    Player player;
    const auto &inv = player.getInventory();
    std::size_t foodCount = 0;
    for (const auto &res : inv) {
        if (res->getType() == ResourceType::FOOD) {
            foodCount++;
        }
    }
    cr_assert_eq(foodCount, 10);
    cr_assert_eq(inv.size(), 10);
}

Test(Player, id_auto_increment)
{
    Player p1;
    Player p2;
    cr_assert_eq(p2.getId(), p1.getId() + 1);
}

Test(Player, setPosition)
{
    Player player;
    player.setPosition(5, 10);
    cr_assert_eq(player.getX(), 5);
    cr_assert_eq(player.getY(), 10);
}

Test(Player, setLevel)
{
    Player player;
    player.setLevel(5);
    cr_assert_eq(player.getLevel(), 5);
}

Test(Player, setTeam)
{
    Player player;
    player.setTeam("team_name");
    cr_assert_eq(player.getTeam(), "team_name");
}

Test(Player, setDirection)
{
    Player player;
    player.setDirection(Player::Direction::East);
    cr_assert_eq(player.getDirection(), Player::Direction::East);
}

Test(Player, setFrozen)
{
    Player player;
    cr_assert_eq(player.isFrozen(), false);
    player.setFrozen(true);
    cr_assert_eq(player.isFrozen(), true);
    player.setFrozen(false);
    cr_assert_eq(player.isFrozen(), false);
}

Test(Player, moveForward_north_wraps)
{
    Player player;
    Map map(10, 10);
    player.setPosition(5, 0);
    player.moveForward(map);
    cr_assert_eq(player.getY(), map.height() - 1);
    cr_assert_eq(player.getX(), 5);
}

Test(Player, moveForward_north_normal)
{
    Player player;
    Map map(10, 10);
    player.setPosition(5, 5);
    player.moveForward(map);
    cr_assert_eq(player.getY(), 4);
    cr_assert_eq(player.getX(), 5);
}

Test(Player, moveForward_east_wraps)
{
    Player player;
    Map map(10, 10);
    player.setDirection(Player::Direction::East);
    player.setPosition(9, 5);
    player.moveForward(map);
    cr_assert_eq(player.getX(), 0);
    cr_assert_eq(player.getY(), 5);
}

Test(Player, moveForward_east_normal)
{
    Player player;
    Map map(10, 10);
    player.setDirection(Player::Direction::East);
    player.setPosition(3, 5);
    player.moveForward(map);
    cr_assert_eq(player.getX(), 4);
}

Test(Player, moveForward_south_wraps)
{
    Player player;
    Map map(10, 10);
    player.setDirection(Player::Direction::South);
    player.setPosition(5, 9);
    player.moveForward(map);
    cr_assert_eq(player.getY(), 0);
    cr_assert_eq(player.getX(), 5);
}

Test(Player, moveForward_south_normal)
{
    Player player;
    Map map(10, 10);
    player.setDirection(Player::Direction::South);
    player.setPosition(5, 5);
    player.moveForward(map);
    cr_assert_eq(player.getY(), 6);
}

Test(Player, moveForward_west_wraps)
{
    Player player;
    Map map(10, 10);
    player.setDirection(Player::Direction::West);
    player.setPosition(0, 5);
    player.moveForward(map);
    cr_assert_eq(player.getX(), map.width() - 1);
    cr_assert_eq(player.getY(), 5);
}

Test(Player, moveForward_west_normal)
{
    Player player;
    Map map(10, 10);
    player.setDirection(Player::Direction::West);
    player.setPosition(5, 5);
    player.moveForward(map);
    cr_assert_eq(player.getX(), 4);
}

Test(Player, moveForward_empty_map_no_op)
{
    Player player;
    Map map;
    player.setPosition(0, 0);
    player.moveForward(map);
    cr_assert_eq(player.getX(), 0);
    cr_assert_eq(player.getY(), 0);
}

Test(Player, moveForward_north_from_north_direction)
{
    Player player;
    Map map(5, 5);
    player.setPosition(2, 2);
    cr_assert_eq(player.getDirection(), Player::Direction::North);
    player.moveForward(map);
    cr_assert_eq(player.getY(), 1);
}

Test(Player, turnRight_cycles)
{
    Player player;
    cr_assert_eq(player.getDirection(), Player::Direction::North);
    player.turnRight();
    cr_assert_eq(player.getDirection(), Player::Direction::East);
    player.turnRight();
    cr_assert_eq(player.getDirection(), Player::Direction::South);
    player.turnRight();
    cr_assert_eq(player.getDirection(), Player::Direction::West);
    player.turnRight();
    cr_assert_eq(player.getDirection(), Player::Direction::North);
}

Test(Player, turnLeft_cycles)
{
    Player player;
    cr_assert_eq(player.getDirection(), Player::Direction::North);
    player.turnLeft();
    cr_assert_eq(player.getDirection(), Player::Direction::West);
    player.turnLeft();
    cr_assert_eq(player.getDirection(), Player::Direction::South);
    player.turnLeft();
    cr_assert_eq(player.getDirection(), Player::Direction::East);
    player.turnLeft();
    cr_assert_eq(player.getDirection(), Player::Direction::North);
}
