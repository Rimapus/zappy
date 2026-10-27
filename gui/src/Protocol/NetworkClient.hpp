/*
 * EPITECH PROJECT, 2026
 * NetworkClient.hpp
 * File description:
 * high level non-blocking protocol client
 */

#pragma once

#include "Buffer.hpp"
#include "TcpSocket.hpp"
#include <cstdint>
#include <string>
#include <vector>

/* @brief TcpSocket and a line Buffer together to expose whole lines */
class NetworkClient
{
public:
    /**
     * @brief Connect to a Zappy server.
     *
     * @param host The server hostname or IP address
     * @param port The server port
     */
    void connect(const std::string &host, std::uint16_t port);

    /**
     * @brief Read everything currently available and split it into lines.
     *
     * @return The complete lines received since the last poll (may be empty).
     */
    std::vector<std::string> poll(void);

    /**
     * @brief Send a raw line to the server, appending a newline.
     *
     * @param line The line to send (without trailing newline)
     */
    void send(const std::string &line);

    /**
     * @brief Check whether the client is still connected.
     *
     * @return true while the socket is open.
     */
    bool isConnected(void) const noexcept;

private:
    TcpSocket _socket;
    Buffer _buffer;
};
