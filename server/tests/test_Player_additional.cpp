/*
** EPITECH PROJECT, 2026
** test_Player_additional.cpp
** File description:
** Additional tests for Player
*/

#include "Player.hpp"
#include "Client.hpp"
#include "Data.hpp"
#include "GameEvent.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(Player, addNewEvent_limited_to_9)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    for (int i = 0; i < 9; i++) {
        client->getPlayer().addNewEvent(data, 10, client, []() {});
    }

    cr_assert_eq(data.getGameEvents().size(), 1);

    client->getPlayer().addNewEvent(data, 10, client, []() {});
    cr_assert_eq(data.getGameEvents().size(), 1);

    test::closeTestFd(fd);
}

Test(Player, frozen_player_does_not_schedule_event)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    client->getPlayer().setFrozen(true);
    auto eventsBefore = data.getGameEvents().size();

    client->getPlayer().addNewEvent(data, 10, client, []() {});
    cr_assert_eq(data.getGameEvents().size(), eventsBefore);

    test::closeTestFd(fd);
}

Test(Player, resumeEvents_after_unfreeze_schedules_event)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    client->getPlayer().setFrozen(true);

    client->getPlayer().addNewEvent(data, 10, client, []() {});
    cr_assert_eq(data.getGameEvents().size(), 0);

    client->getPlayer().setFrozen(false);
    client->getPlayer().resumeEvents(data);
    cr_assert_eq(data.getGameEvents().size(), 1);

    test::closeTestFd(fd);
}

Test(Player, pauseEvent_saves_event)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    bool called = false;

    GameEvent event([&called]() { called = true; }, 10);
    client->getPlayer().pauseEvent(std::move(event));

    test::closeTestFd(fd);
}

Test(Player, resumeEvents_does_nothing_when_frozen)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    client->getPlayer().setFrozen(true);
    client->getPlayer().resumeEvents(data);

    test::closeTestFd(fd);
}

Test(Player, resumeEvents_does_nothing_with_no_paused_or_pending)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();

    client->getPlayer().resumeEvents(data);

    test::closeTestFd(fd);
}

Test(Player, getTeam_returns_empty_string_by_default)
{
    Player player;
    cr_assert_eq(player.getTeam(), "");
}

Test(Player, getTeam_after_setTeam)
{
    Player player;
    player.setTeam("my_team");
    cr_assert_eq(player.getTeam(), "my_team");
}
