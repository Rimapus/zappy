/*
** EPITECH PROJECT, 2026
** test_StringUtils.cpp
** File description:
** test tring utils
*/

#include "StringUtils.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <string>

using namespace server;

Test(StringUtils, toLowerCase_mixed_case)
{
    std::string result = toLowerCase("HeLLo WoRLd");
    cr_assert_str_eq(result.c_str(), "hello world");
}

Test(StringUtils, toLowerCase_already_lower)
{
    std::string result = toLowerCase("hello world");
    cr_assert_str_eq(result.c_str(), "hello world");
}

Test(StringUtils, toLowerCase_all_upper)
{
    std::string result = toLowerCase("HELLO");
    cr_assert_str_eq(result.c_str(), "hello");
}

Test(StringUtils, toLowerCase_empty_string)
{
    std::string result = toLowerCase("");
    cr_assert_str_eq(result.c_str(), "");
}

Test(StringUtils, toLowerCase_numbers_and_symbols)
{
    std::string result = toLowerCase("ABC123!@#");
    cr_assert_str_eq(result.c_str(), "abc123!@#");
}
