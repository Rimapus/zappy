/*
** EPITECH PROJECT, 2026
** Socket.hpp
** File description:
** TCP client socket wrapper for the AI
*/

#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

/// @namespace ai
/// @brief Zappy AI internals: socket layer and server protocol.
namespace ai {

/**
 * @class SocketException
 * @brief Exception thrown on socket-level errors (connect, send, recv…).
 *
 * Inherits from std::runtime_error so it can be caught generically.
 * The errno value at the time of the failure is preserved in errorNumber().
 */
class SocketException : public std::runtime_error
{
public:
    /**
     * @brief Construct a SocketException.
     * @param message      Human-readable description of the failure.
     * @param errorNumber  errno value at the time of the failure (0 if n/a).
     */
    explicit SocketException(const std::string &message, int errorNumber = 0) noexcept;

    /**
     * @brief Return the errno value captured at construction time.
     * @return errno value, or 0 if not applicable.
     */
    int errorNumber() const noexcept;

private:
    int _errorNumber;
};

/**
 * @class SocketClosed
 * @brief Exception thrown when the remote end closes the connection.
 *
 * Distinct from SocketException so callers can handle a clean server
 * shutdown differently from a network error.
 */
class SocketClosed : public std::exception
{
public:
    SocketClosed() noexcept = default;

    /// @return Fixed message "socket closed by server".
    const char *what() const noexcept override { return "socket closed by server"; }
};

/**
 * @class SocketTimeout
 * @brief Exception thrown when a poll() timeout expires before data is ready.
 *
 * The caller can catch this to decide whether to retry, wait, or give up.
 */
class SocketTimeout : public std::exception
{
public:
    SocketTimeout() noexcept = default;

    /// @return Fixed message "socket operation timed out".
    const char *what() const noexcept override { return "socket operation timed out"; }
};

/**
 * @class Socket
 * @brief RAII wrapper around a TCP client socket with poll-based timeouts.
 *
 * Send and receive operations use poll() internally so they never block
 * indefinitely: if the fd is not ready within @p timeoutMs milliseconds,
 * SocketTimeout is thrown instead of hanging the calling thread.
 *
 * The class is move-only (copy is deleted) to enforce single ownership of the
 * underlying file descriptor.
 *
 * @note All methods throw SocketException on error, SocketClosed when the
 *       remote end closes the connection, and SocketTimeout on poll timeout.
 */
class Socket
{
public:
    /// Poll timeout: -1 means block until the fd is ready (no busy-wait).
    /// POLLHUP/POLLERR still fires immediately if the server closes/crashes.
    static constexpr int DEFAULT_TIMEOUT_MS = -1;

    /// @brief Construct an unconnected socket (fd = -1).
    Socket();

    /// @brief Disconnect and close the file descriptor if open.
    ~Socket();

    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    /**
     * @brief Move constructor — transfers ownership of the file descriptor.
     * @param other  Source socket; left in a disconnected state.
     */
    Socket(Socket &&other) noexcept;

    /**
     * @brief Move assignment — closes current fd, then takes ownership.
     * @param other  Source socket; left in a disconnected state.
     */
    Socket &operator=(Socket &&other) noexcept;

    /**
     * @brief Connect to a remote host using TCP.
     *
     * Resolves @p host via getaddrinfo (supports both IPv4 and IPv6) and
     * attempts each result in order until one succeeds.
     *
     * @param host  Hostname or IP address of the server.
     * @param port  Port number as a string (e.g. "4242").
     * @throws SocketException if already connected, DNS resolution fails, or
     *         all connection attempts fail.
     */
    void connect(const std::string &host, const std::string &port);

    /**
     * @brief Close the socket.  Safe to call on an already-disconnected socket.
     */
    void disconnect();

    /**
     * @brief Send the entire message to the server.
     *
     * Uses poll() before each ::send() call to avoid blocking indefinitely.
     * Loops until all bytes are written.
     * Uses MSG_NOSIGNAL so that a broken pipe does not raise SIGPIPE.
     *
     * @param message    Data to send (no implicit newline appended).
     * @param timeoutMs  Maximum milliseconds to wait for the fd to be writable
     *                   on each iteration (default: DEFAULT_TIMEOUT_MS).
     * @throws SocketException  if not connected or a send error occurs.
     * @throws SocketClosed     if EPIPE or ECONNRESET is returned.
     * @throws SocketTimeout    if poll() times out waiting for write-readiness.
     */
    void send(const std::string &message, int timeoutMs = DEFAULT_TIMEOUT_MS) const;

    /**
     * @brief Read up to @p bufferSize bytes from the server.
     *
     * Uses poll() before ::recv() to avoid blocking indefinitely.
     * May return less than @p bufferSize bytes; the caller (ServerConnection)
     * is responsible for buffering partial reads.
     *
     * @param bufferSize  Maximum number of bytes to read (default: 4096).
     * @param timeoutMs   Maximum milliseconds to wait for data to arrive
     *                    (default: DEFAULT_TIMEOUT_MS).
     * @return The received data as a string (never empty).
     * @throws SocketException  if not connected or recv fails.
     * @throws SocketClosed     if the server closed the connection (recv == 0).
     * @throws SocketTimeout    if poll() times out waiting for data.
     */
    std::string receive(std::size_t bufferSize = 4096, int timeoutMs = DEFAULT_TIMEOUT_MS) const;

    /**
     * @brief Return whether the socket currently holds an open file descriptor.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const noexcept { return _fd >= 0; }

    /**
     * @brief Return the raw file descriptor.
     * @return fd >= 0 if connected, -1 otherwise.
     */
    int fd() const noexcept { return _fd; }

private:
    /**
     * @brief Wait until the fd is ready for @p events using poll().
     *
     * @param events     poll event mask (POLLIN or POLLOUT).
     * @param timeoutMs  Maximum wait in milliseconds.
     * @return revents mask returned by poll().
     * @throws SocketException  if poll() itself fails.
     * @throws SocketTimeout    if poll() returns 0 (timeout expired).
     */
    short pollFd(short events, int timeoutMs) const;

    int _fd; ///< Underlying file descriptor (-1 when disconnected).
};

} // namespace ai
