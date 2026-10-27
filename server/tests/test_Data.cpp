/*
** EPITECH PROJECT, 2026
** test_Data.cpp
** File description:
** Tests for ServerData and GameEvent scheduling
*/

#include "Data.hpp"
#include "GameEvent.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <functional>

using namespace server;

Test(ServerData, default_constructor)
{
    auto data = test::createTestServerData(500, 20, 30);
    cr_assert_eq(data.getFrequency(), 500);
    cr_assert_eq(data.getMap().width(), 20);
    cr_assert_eq(data.getMap().height(), 30);
    cr_assert_eq(data.getTeams().size(), 1);
    cr_assert_eq(data.getTeams()[0], "team1");
    cr_assert_eq(data.getFinished(), false);
}

Test(ServerData, setFinished)
{
    auto data = test::createTestServerData();
    cr_assert_eq(data.getFinished(), false);
    data.setFinished(true);
    cr_assert_eq(data.getFinished(), true);
    data.setFinished(false);
    cr_assert_eq(data.getFinished(), false);
}

Test(ServerData, setFrequency_reschedules_events)
{
    auto data = test::createTestServerData(100);
    GameEvent event([]() {}, 50);

    event.scheduleTask(data);
    data.getGameEvents().push(event);
    auto firstTime = data.getGameEvents().top()._executionTime;

    data.setFrequency(200);

    auto secondTime = data.getGameEvents().top()._executionTime;
    cr_assert_neq(firstTime, secondTime);
}

Test(ServerData, setFrequency_with_no_events)
{
    auto data = test::createTestServerData(100);
    data.setFrequency(200);
    cr_assert_eq(data.getFrequency(), 200);
}

Test(ServerData, getServerSocket_returns_non_null)
{
    auto data = test::createTestServerData();
    auto &sock = data.getServerSocket();
    cr_assert_not_null(&sock);
}
