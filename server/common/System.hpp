/*
** EPITECH PROJECT, 2026
** System.hpp
** File description:
** Encapsulated system function
*/

#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <vector>

namespace server {

class Socket;

class System
{
public:
    class SystemException : public std::runtime_error
    {
    public:
        SystemException(const std::string &operation, int errorNumber = 0);
        int errorNumber() const noexcept;

    private:
        int _errorNumber;
    };

    static short poll(int fd, short events, std::size_t timeout = 0);
    static void poll(std::vector<struct ::pollfd> &list, std::size_t timeout);
    static std::size_t write(int fd, const std::string &str);
    static std::string read(int fd, std::size_t size);
};

}
