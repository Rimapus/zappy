/*
** EPITECH PROJECT, 2026
** Enum.hpp
** File description:
** Enum class to iterate
*/

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

template <typename E>
concept ContinuousEnum = std::is_enum_v<E> && requires {
    { E::COUNT } -> std::convertible_to<E>;
};

template <ContinuousEnum E>
class EnumIterator
{
    std::underlying_type_t<E> _value;

public:
    explicit EnumIterator(std::underlying_type_t<E> v)
        : _value(v) {}

    E operator*() const { return static_cast<E>(_value); }

    EnumIterator &operator++()
    {
        ++_value;
        return *this;
    }

    bool operator!=(const EnumIterator &other) const { return _value != other._value; }
};

template <ContinuousEnum E>
class EnumRange
{
    std::underlying_type_t<E> _begin;
    std::underlying_type_t<E> _end;

public:
    EnumRange(E begin, E end = E::COUNT)
        : _begin(std::to_underlying(begin)), _end(std::to_underlying(end)) {}

    EnumIterator<E> begin() const { return EnumIterator<E>(_begin); }

    EnumIterator<E> end() const { return EnumIterator<E>(_end); }
};
