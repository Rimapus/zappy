/*
** EPITECH PROJECT, 2026
** Main.hpp
** File description:
** Main include file for the zappy_server
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Config
{
public:
    std::uint16_t _port;
    std::size_t _width;
    std::size_t _height;
    std::vector<std::string> _teamNames;
    std::size_t _clientsNumber;
    std::size_t _frequency;
};
