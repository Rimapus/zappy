/*
** EPITECH PROJECT, 2026
** test_GameEvent.cpp
** File description:
** Tests for GameEvent
*/

#include "GameEvent.hpp"
#include "Data.hpp"
#include "test_helpers.hpp"
#include <chrono>
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <functional>

using namespace server;

Test(GameEvent, constructor_sets_callback_and_timeLimit)
{
    bool called = false;
    GameEvent event([&called]() { called = true; }, 42);

    cr_assert_eq(event._timeLimit, 42);
    cr_assert_neq(event._callback, nullptr);

    event._callback();
    cr_assert_eq(called, true);
}

Test(GameEvent, scheduleTask_sets_execution_time)
{
    auto data = test::createTestServerData();
    GameEvent event([]() {}, 100);

    event.scheduleTask(data);

    cr_assert_neq(event._executionTime.time_since_epoch().count(), 0);
}

Test(GameEvent, rescheduleTask_resets_execution_time)
{
    auto data = test::createTestServerData();
    GameEvent event([]() {}, 100);

    event.scheduleTask(data);
    auto firstTime = event._executionTime;
    event.rescheduleTask(data);

    auto secondTime = event._executionTime;
    cr_assert_geq(secondTime, firstTime);
}

Test(GameEvent, rescheduleTask_does_nothing_if_not_scheduled)
{
    auto data = test::createTestServerData();
    GameEvent event([]() {}, 100);

    event.rescheduleTask(data);

    cr_assert_eq(event._executionTime.time_since_epoch().count(), 0);
}

Test(GameEvent, setOwner_and_getOwner)
{
    auto [client, fd] = test::createTestClient();
    GameEvent event([]() {}, 10);

    event.setOwner(client);

    auto owner = event.getOwner();
    cr_assert_eq(owner, client);
    test::closeTestFd(fd);
}

Test(GameEvent, getOwner_returns_null_when_no_owner)
{
    GameEvent event([]() {}, 10);

    auto owner = event.getOwner();
    cr_assert_eq(owner, nullptr);
}

Test(CompareEventTime, orders_by_execution_time)
{
    GameEvent a([]() {}, 100);
    GameEvent b([]() {}, 50);

    auto now = std::chrono::steady_clock::now();
    a._executionTime = now + std::chrono::seconds(10);
    b._executionTime = now + std::chrono::seconds(5);

    CompareEventTime cmp;
    cr_assert_eq(cmp(a, b), true);
    cr_assert_eq(cmp(b, a), false);
}

Test(CompareEventTime, equal_times_not_less)
{
    GameEvent a([]() {}, 100);
    GameEvent b([]() {}, 50);

    auto now = std::chrono::steady_clock::now();
    a._executionTime = now;
    b._executionTime = now;

    CompareEventTime cmp;
    cr_assert_eq(cmp(a, b), false);
    cr_assert_eq(cmp(b, a), false);
}
