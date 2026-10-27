/*
** EPITECH PROJECT, 2026
** test_ResourceType.cpp
** File description:
** test Ressources type
*/

#include "map/resources/Food.hpp"
#include "map/resources/IResource.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <sstream>
#include <string>

using namespace server;

static void checkToString(ResourceType::Value value, const char *expected)
{
    std::string result(ResourceType::toString(value));
    cr_assert_str_eq(result.c_str(), expected);
}

Test(ResourceType, toString_food)
{
    checkToString(ResourceType::FOOD, "food");
}

Test(ResourceType, toString_linemate)
{
    checkToString(ResourceType::LINEMATE, "linemate");
}

Test(ResourceType, toString_deraumere)
{
    checkToString(ResourceType::DERAUMERE, "deraumere");
}

Test(ResourceType, toString_sibur)
{
    checkToString(ResourceType::SIBUR, "sibur");
}

Test(ResourceType, toString_mendiane)
{
    checkToString(ResourceType::MENDIANE, "mendiane");
}

Test(ResourceType, toString_phiras)
{
    checkToString(ResourceType::PHIRAS, "phiras");
}

Test(ResourceType, toString_thystame)
{
    checkToString(ResourceType::THYSTAME, "thystame");
}

Test(ResourceType, toString_count)
{
    cr_assert_eq(ResourceType::toString(ResourceType::COUNT).size(), 0);
}

Test(ResourceType, toString_from_value)
{
    ResourceType type(ResourceType::SIBUR);
    std::string result(type.toString());
    cr_assert_str_eq(result.c_str(), "sibur");
}

Test(ResourceType, fromString_food)
{
    cr_assert_eq(ResourceType::fromString("food"), ResourceType::FOOD);
}

Test(ResourceType, fromString_linemate)
{
    cr_assert_eq(ResourceType::fromString("linemate"), ResourceType::LINEMATE);
}

Test(ResourceType, fromString_deraumere)
{
    cr_assert_eq(ResourceType::fromString("deraumere"), ResourceType::DERAUMERE);
}

Test(ResourceType, fromString_sibur)
{
    cr_assert_eq(ResourceType::fromString("sibur"), ResourceType::SIBUR);
}

Test(ResourceType, fromString_mendiane)
{
    cr_assert_eq(ResourceType::fromString("mendiane"), ResourceType::MENDIANE);
}

Test(ResourceType, fromString_phiras)
{
    cr_assert_eq(ResourceType::fromString("phiras"), ResourceType::PHIRAS);
}

Test(ResourceType, fromString_thystame)
{
    cr_assert_eq(ResourceType::fromString("thystame"), ResourceType::THYSTAME);
}

Test(ResourceType, fromString_invalid)
{
    cr_assert_eq(ResourceType::fromString("unknown"), ResourceType::COUNT);
    cr_assert_eq(ResourceType::fromString(""), ResourceType::COUNT);
    cr_assert_eq(ResourceType::fromString("Foooood"), ResourceType::COUNT);
}

Test(ResourceType, equality)
{
    ResourceType a(ResourceType::FOOD);
    ResourceType b(ResourceType::FOOD);
    ResourceType c(ResourceType::LINEMATE);
    cr_assert_eq(a, b);
    cr_assert_neq(a, c);
}

Test(ResourceType, ostream_operator)
{
    ResourceType type(ResourceType::MENDIANE);
    std::ostringstream os;
    os << type;
    std::string result = os.str();
    cr_assert_str_eq(result.c_str(), "mendiane");
}

Test(ResourceType, iresource_ostream)
{
    Food food;
    std::ostringstream os;
    os << food;
    std::string result = os.str();
    cr_assert_str_eq(result.c_str(), "food");
}
