/*
** EPITECH PROJECT, 2026
** test_Enum.cpp
** File description:
** test enum iterator
*/

#include "Enum.hpp"
#include "map/resources/IResource.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <cstddef>
#include <utility>

using namespace server;

Test(Enum, range_iterates_over_resource_types)
{
    std::size_t count = 0;
    for (auto type : EnumRange(ResourceType::FOOD, ResourceType::COUNT)) {
        (void)type;
        count++;
    }
    cr_assert_eq(count, 7);
}

Test(Enum, range_includes_all_values)
{
    bool found[7] = {false};
    for (auto type : EnumRange(ResourceType::FOOD)) {
        found[std::to_underlying(type)] = true;
    }
    for (int i = 0; i < 7; i++) {
        cr_assert_eq(found[i], true);
    }
}

Test(Enum, range_count_is_right)
{
    cr_assert_eq(ResourceType::COUNT, static_cast<ResourceType::Value>(7));
}

Test(Enum, iterator_dereference)
{
    EnumIterator<ResourceType::Value> it(0);
    cr_assert_eq(*it, ResourceType::FOOD);
}
