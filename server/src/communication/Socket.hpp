/*
** EPITECH PROJECT, 2026
** Socket.hpp
** File description:
** Socket class
*/

#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <utility>
#include <vector>

namespace server {

class SocketException : public std::runtime_error
{
public:
    SocketException(const std::string &message, int errorNumber = 0) noexcept;
    int errorNumber() const noexcept;

private:
    int _errorNumber;
};

class SocketClosed : public std::exception
{
public:
    SocketClosed() noexcept;
    ~SocketClosed() override = default;
    const char *what() const noexcept override;

private:
    std::string _message;
};

class Socket
{
public:
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    explicit Socket(int domain, int type, int protocol);
    explicit Socket(int fd);
    Socket(Socket &&);
    Socket &operator=(Socket &&) noexcept;
    ~Socket();

    short poll(short events,
               std::size_t timeout = 0) const;
    std::size_t write(const std::string &str) const;
    void bind(const sockaddr &addr) const;
    void setReuseAddress(void) const;
    std::string read(std::size_t size) const;

    void listen(int backlog = 4096) const;
    std::pair<std::unique_ptr<Socket>, std::unique_ptr<struct sockaddr>> accept(void) const;

    static std::vector<std::optional<short>> pollMultiple(const std::vector<std::shared_ptr<Socket>> &list, short events,
                                                          std::size_t timeout = 0);

private:
    int _fd{-1};
};

}
