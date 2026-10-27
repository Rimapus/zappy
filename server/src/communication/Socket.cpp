/*
** EPITECH PROJECT, 2026
** Socket.cpp
** File description:
** Socket class
*/

#include "Socket.hpp"
#include "System.hpp"
#include <cerrno>
#include <cstddef>
#include <memory>
#include <netinet/in.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

#ifdef __linux__
#include <asm-generic/socket.h>
#endif

namespace server {

SocketException::SocketException(const std::string &message, int errorNumber) noexcept
    : std::runtime_error(message), _errorNumber(errorNumber)
{
}

int SocketException::errorNumber() const noexcept
{
    return _errorNumber;
}

Socket::Socket(int domain, int type, int protocol)
    : _fd(socket(domain, type, protocol))
{
    if (_fd == -1) {
        throw SocketException("socket() failed", errno);
    }
}

void Socket::bind(const sockaddr &addr) const
{
    if (::bind(_fd, &addr, sizeof(addr)) == -1) {
        throw SocketException("bind() failed", errno);
    }
}

void Socket::setReuseAddress(void) const
{
    const int reuseAddr = 1;
    if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1) {
        throw SocketException("setsockopt(SO_REUSEADDR) failed", errno);
    }
}

Socket::Socket(int fd)
{
    if (fd < 0) {
        throw SocketException("socket fd is invalid", EBADF);
    }

    struct stat statbuf{};

    if (fstat(fd, &statbuf) == -1) {
        throw SocketException("fstat() failed for socket fd", errno);
    }
    if (!S_ISSOCK(statbuf.st_mode)) {
        throw SocketException("fd is not a socket", ENOTSOCK);
    }
    _fd = fd;
}

Socket::~Socket()
{
    if (_fd >= 0) {
        ::close(_fd);
    }
}

Socket::Socket(Socket &&other)
    : _fd(other._fd)
{
    other._fd = -1;
};

Socket &Socket::operator=(Socket &&other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (_fd >= 0) {
        ::close(_fd);
    }

    _fd = other._fd;
    other._fd = -1;
    return *this;
};

short Socket::poll(short events, std::size_t timeout) const
{
    return System::poll(_fd, events, timeout);
}

std::size_t Socket::write(const std::string &str) const
{
    if (_fd < 0) {
        throw SocketException("socket fd is invalid", EBADF);
    }

    auto pollRes = poll(POLLOUT);
    if (pollRes & POLLERR || pollRes & POLLHUP) {
        throw SocketClosed();
    }
    if (pollRes & POLLOUT) {
        return System::write(_fd, str);
    }
    return 0;
}

std::string Socket::read(std::size_t size) const
{
    if (_fd < 0) {
        throw SocketException("socket fd is invalid", EBADF);
    }

    if (size == 0) {
        return "";
    }

    auto pollRes = poll(POLLIN);

    if (pollRes & POLLERR || pollRes & POLLHUP) {
        std::string read;
        if (pollRes & POLLIN) {
            read = System::read(_fd, size);
        }
        if (read.empty()) {
            throw SocketClosed();
        }
        return read;
    }
    if (pollRes & POLLIN) {
        return System::read(_fd, size);
    }
    return "";
};

void Socket::listen(int backlog) const
{
    if (::listen(_fd, backlog) == -1) {
        throw SocketException("listen() failed", errno);
    }
}

std::pair<std::unique_ptr<Socket>, std::unique_ptr<struct sockaddr>>
Socket::accept() const
{
    sockaddr_in addr{};
    socklen_t sockLen{sizeof(addr)};
    int fd = -1;

    fd = ::accept(_fd, reinterpret_cast<struct sockaddr *>(&addr), &sockLen);

    if (fd == -1) {
        throw SocketException("accept() failed", errno);
    }

    auto res1 = std::make_unique<Socket>(fd);
    auto res2 = std::make_unique<struct sockaddr>(
        *reinterpret_cast<struct sockaddr *>(&addr));

    return {std::move(res1), std::move(res2)};
}

std::vector<std::optional<short>> Socket::pollMultiple(const std::vector<std::shared_ptr<Socket>> &list, short events,
                                                       std::size_t timeout)
{
    std::vector<struct pollfd> fdList;
    for (const auto &socket : list) {
        fdList.push_back({socket->_fd, events, 0});
    }
    System::poll(fdList, timeout);
    std::vector<std::optional<short>> res{};
    for (const auto &fd : fdList) {
        if (fd.revents) {
            res.push_back(fd.revents);
        } else {
            res.push_back(std::nullopt);
        }
    }
    return res;
}

SocketClosed::SocketClosed() noexcept
    : _message("socket closed unexpectedly")
{
}

const char *SocketClosed::what() const noexcept
{
    return _message.c_str();
}

}
