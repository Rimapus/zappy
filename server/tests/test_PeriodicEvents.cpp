/*
** EPITECH PROJECT, 2026
** test_PeriodicEvents.cpp
** File description:
** Tests for periodic events
*/

#include "PeriodicEvents.hpp"
#include "test_helpers.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

using namespace server;

Test(PeriodicEvents, process_schedules_resource_spawn_and_food_consumption)
{
    auto data = test::createTestServerData();

    cr_assert_eq(data.getPeriodicEvents().getRefilled(), true);
    cr_assert_eq(data.getPeriodicEvents().getFoodConsumed(), true);

    processPeriodicEvents(data);

    cr_assert_eq(data.getPeriodicEvents().getRefilled(), false);
    cr_assert_eq(data.getPeriodicEvents().getFoodConsumed(), false);

    auto &events = data.getGameEvents();
    cr_assert_eq(events.size(), 2);
}

Test(PeriodicEvents, process_idempotent_when_flags_false)
{
    auto data = test::createTestServerData();
    data.getPeriodicEvents().setRefilled(false);
    data.getPeriodicEvents().setFoodConsumed(false);

    processPeriodicEvents(data);

    cr_assert_eq(data.getGameEvents().size(), 0);
}

Test(PeriodicEvents, food_consumption_kills_client_without_food)
{
    auto data = test::createTestServerData();
    auto [client, fd] = test::createTestClient();
    test::addToActiveClients(data, client);

    auto &inv = client->getPlayer().getInventory();
    inv.clear();

    processPeriodicEvents(data);

    auto &events = data.getGameEvents();
    cr_assert_geq(events.size(), 1);

    while (!events.empty()) {
        auto event = events.top();
        events.pop();
        event._callback();
    }

    cr_assert_eq(client->shouldClose(), true);
    test::closeTestFd(fd);
}

Test(PeriodicEvents, pushScheduledEvent_adds_to_queue)
{
    auto data = test::createTestServerData();
    bool called = false;

    pushScheduledEvent(data, [&called]() { called = true; }, 50);

    cr_assert_eq(data.getGameEvents().size(), 1);

    auto event = data.getGameEvents().top();
    data.getGameEvents().pop();
    event._callback();
    cr_assert_eq(called, true);
}
