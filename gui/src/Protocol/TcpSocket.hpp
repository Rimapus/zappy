/*
 * EPITECH PROJECT, 2026
 * TcpSocket.hpp
 * File description:
 * tcp socket wrapper
 */

#pragma once

#include <cstdint>
#include <string>

/* @brief Non-blocking POSIX TCP client socket */
class TcpSocket
{
public:
    TcpSocket() = default;
    ~TcpSocket();

    TcpSocket(const TcpSocket &) = delete;
    TcpSocket &operator=(const TcpSocket &) = delete;

    /**
     * @brief Connect to a TCP server.
     *
     * @param host The server hostname or IP address
     * @param port The server port
     */
    void connect(const std::string &host, std::uint16_t port);

    /**
     * @brief Send data to the server.
     *
     * @param data The data to send
     */
    void send(const std::string &data);

    /**
     * @brief Receive data from the server.
     *
     * @param buf Buffer where the received data will be stored
     * @param maxLen Maximum number of bytes to receive
     */
    int recv(char *buf, std::size_t maxLen);

    /**
     * @brief Disconnect from the server and close the socket.
     */
    void disconnect();

    /**
     * @brief Get the socket file descriptor.
     *
     * @return The socket file descriptor, or -1 if not connected
     */
    int getFd() const noexcept;

    /**
     * @brief Check if the socket is currently connected.
     *
     * @return true if connected, false otherwise
     */
    bool isConnected() const noexcept;

private:
    int _fd = -1;
};
