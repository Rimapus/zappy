/*
 * EPITECH PROJECT, 2026
 * NetworkClient.cpp
 * File description:
 * high level non-blocking protocol client
 */

#include "NetworkClient.hpp"

/**
 * @brief Connect to a Zappy server.
 *
 * @param host The server hostname or IP address
 * @param port The server port
 */
void NetworkClient::connect(const std::string &host, std::uint16_t port)
{
    _socket.connect(host, port);
}

/**
 * @brief Drain the socket and return every complete line received.
 *
 * @return The lines received since the last poll (without trailing newline).
 */
std::vector<std::string> NetworkClient::poll()
{
    std::vector<std::string> lines;
    char raw[4096];
    int n = _socket.recv(raw, sizeof(raw));

    if (n > 0) {
        _buffer.append(raw, static_cast<std::size_t>(n));
    }
    std::string line;
    while (_buffer.popLine(line)) {
        lines.push_back(line);
    }
    return lines;
}

/**
 * @brief Send a line to the server, terminated by a newline.
 *
 * @param line The line to send (without trailing newline)
 */
void NetworkClient::send(const std::string &line)
{
    _socket.send(line + "\n");
}

/**
 * @brief Report whether the underlying socket is connected.
 *
 * @return true while the socket is open.
 */
bool NetworkClient::isConnected() const noexcept
{
    return _socket.isConnected();
}
