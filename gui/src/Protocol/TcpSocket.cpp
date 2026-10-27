/*
 * EPITECH PROJECT, 2026
 * TcpSocket.cpp
 * File description:
 * tcp socket wrapper
 */

#include "TcpSocket.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

TcpSocket::~TcpSocket()
{
    disconnect();
}

/**
 * @brief Connect to a remote TCP server and switch the socket to non-blocking mode.
 *
 * @param host Hostname or IP address
 * @param port Port number
 */
void TcpSocket::connect(const std::string &host, std::uint16_t port)
{
    struct addrinfo *result = nullptr;
    std::string portStr = std::to_string(port);
    int fd = -1;

    if (isConnected()) {
        throw std::runtime_error("TcpSocket: already connected");
    }
    if (getaddrinfo(host.c_str(), portStr.c_str(), nullptr, &result) != 0) {
        throw std::runtime_error("TcpSocket: getaddrinfo failed: " + std::string(std::strerror(errno)));
    }
    for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
        fd = ::socket(rp->ai_family, SOCK_STREAM, 0);
        if (fd == -1) {
            continue;
        }
        if (::connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        ::close(fd);
        fd = -1;
    }
    freeaddrinfo(result);
    if (fd == -1) {
        throw std::runtime_error("TcpSocket: could not connect to " + host + ":" + portStr);
    }
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        ::close(fd);
        throw std::runtime_error("TcpSocket: fcntl failed: " + std::string(std::strerror(errno)));
    }
    _fd = fd;
}

/**
 * @brief Send data to the server (handles partial writes).
 *
 * @param data Data to send
 */
void TcpSocket::send(const std::string &data)
{
    std::size_t sent = 0;

    if (!isConnected()) {
        throw std::runtime_error("TcpSocket: not connected");
    }
    while (sent < data.size()) {
        ssize_t n = ::send(_fd, data.c_str() + sent, data.size() - sent, 0);
        if (n == -1) {
            throw std::runtime_error("TcpSocket: send failed: " + std::string(std::strerror(errno)));
        }
        sent += static_cast<std::size_t>(n);
    }
}

/**
 * @brief Read available bytes from the socket (non-blocking).
 *
 * @param buf    Buffer to write into
 * @param maxLen Maximum number of bytes to read
 * @return Number of bytes read, 0 if nothing available (EAGAIN)
 */
int TcpSocket::recv(char *buf, std::size_t maxLen)
{
    if (!isConnected()) {
        throw std::runtime_error("TcpSocket: not connected");
    }
    ssize_t n = ::recv(_fd, buf, maxLen, 0);
    if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return 0;
    }
    if (n == -1) {
        throw std::runtime_error("TcpSocket: recv failed: " + std::string(std::strerror(errno)));
    }
    if (n == 0) {
        disconnect();
        throw std::runtime_error("TcpSocket: server closed connection");
    }
    return static_cast<int>(n);
}

/**
 * @brief Close the socket and mark as disconnected.
 */
void TcpSocket::disconnect()
{
    if (isConnected()) {
        ::close(_fd);
        _fd = -1;
    }
}

/**
 * @brief Return the underlying file descriptor.
 *
 * @return File descriptor, or -1 if not connected
 */
int TcpSocket::getFd() const noexcept
{
    return _fd;
}

/**
 * @brief Return whether the socket is currently connected.
 *
 * @return true if connected, false otherwise
 */
bool TcpSocket::isConnected() const noexcept
{
    return _fd != -1;
}
