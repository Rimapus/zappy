/*
** EPITECH PROJECT, 2026
** Socket.cpp
** File description:
** TCP client socket wrapper for the AI
*/

#include "Socket.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace ai {

// SocketException

SocketException::SocketException(const std::string &message, int errorNumber) noexcept
    : std::runtime_error(message), _errorNumber(errorNumber)
{
}

int SocketException::errorNumber() const noexcept
{
    return _errorNumber;
}

// Socket

Socket::Socket() : _fd(-1)
{
}

Socket::~Socket()
{
    disconnect();
}

Socket::Socket(Socket &&other) noexcept : _fd(other._fd)
{
    other._fd = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept
{
    if (this == &other) {
        return *this;
    }
    disconnect();
    _fd = other._fd;
    other._fd = -1;
    return *this;
}

void Socket::connect(const std::string &host, const std::string &port)
{
    if (_fd >= 0) {
        throw SocketException("socket already connected");
    }

    struct addrinfo hints{};
    hints.ai_family = AF_UNSPEC;     // accept IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    struct addrinfo *results = nullptr;
    int ret = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &results);
    if (ret != 0) {
        throw SocketException(std::string("getaddrinfo() failed: ") + ::gai_strerror(ret));
    }

    // Try each addrinfo result in order; stop at the first successful connect.
    int fd = -1;
    for (struct addrinfo *rp = results; rp != nullptr; rp = rp->ai_next) {
        fd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) {
            continue;
        }
        if (::connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        ::close(fd);
        fd = -1;
    }
    ::freeaddrinfo(results);

    if (fd == -1) {
        throw SocketException(std::string("connect() failed: ") + std::strerror(errno), errno);
    }
    _fd = fd;
}

void Socket::disconnect()
{
    if (_fd >= 0) {
        ::close(_fd);
        _fd = -1;
    }
}

void Socket::send(const std::string &message, int timeoutMs) const
{
    if (_fd < 0) {
        throw SocketException("socket is not connected", EBADF);
    }

    const char *ptr = message.data();
    std::size_t remaining = message.size();

    // Loop until all bytes are sent (::send may write less than requested).
    while (remaining > 0) {
        pollFd(POLLOUT, timeoutMs);

        ssize_t sent = ::send(_fd, ptr, remaining, MSG_NOSIGNAL);
        if (sent == -1) {
            if (errno == EPIPE || errno == ECONNRESET) {
                throw SocketClosed();
            }
            throw SocketException(std::string("send() failed: ") + std::strerror(errno), errno);
        }
        ptr += sent;
        remaining -= static_cast<std::size_t>(sent);
    }
}

std::string Socket::receive(std::size_t bufferSize, int timeoutMs) const
{
    if (_fd < 0) {
        throw SocketException("socket is not connected", EBADF);
    }

    pollFd(POLLIN, timeoutMs);

    std::string buffer(bufferSize, '\0');
    ssize_t n = ::recv(_fd, &buffer[0], bufferSize, 0);

    if (n < 0) {
        throw SocketException(std::string("recv() failed: ") + std::strerror(errno), errno);
    }
    if (n == 0) {
        // Remote end closed the connection gracefully.
        throw SocketClosed();
    }

    buffer.resize(static_cast<std::size_t>(n));
    return buffer;
}

// ─── Internal ────────────────────────────────────────────────────────────────

short Socket::pollFd(short events, int timeoutMs) const
{
    struct pollfd pfd{};
    pfd.fd = _fd;
    pfd.events = events;

    int ret = ::poll(&pfd, 1, timeoutMs);
    if (ret == -1) {
        throw SocketException(std::string("poll() failed: ") + std::strerror(errno), errno);
    }
    if (ret == 0) {
        throw SocketTimeout();
    }
    if ((pfd.revents & POLLHUP) || (pfd.revents & POLLERR)) {
        throw SocketClosed();
    }
    return pfd.revents;
}

} // namespace ai
