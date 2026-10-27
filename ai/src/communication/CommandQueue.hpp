/*
** EPITECH PROJECT, 2026
** CommandQueue.hpp
** File description:
** Client-side command queue for the Zappy AI
*/

#pragma once

#include "ServerConnection.hpp"

#include <cstddef>
#include <deque>
#include <functional>
#include <string>

namespace ai {

/**
 * @class CommandQueue
 * @brief Client-side command pipeline built on top of ServerConnection.
 *
 * The Zappy server buffers at most 10 commands per player and answers them in
 * the exact order it received them (subject §4). CommandQueue lets the AI think
 * in terms of *intent* — it may enqueue as many commands as it likes — while
 * this class takes care of:
 * - keeping at most ServerConnection::MAX_PENDING_COMMANDS in flight,
 * - holding the surplus locally until a slot frees up,
 * - dispatching each server response to the command that produced it (FIFO),
 *   through an optional per-command callback.
 *
 * Two internal queues model the lifecycle of a command:
 * - **queued**   — pushed by the AI, not yet sent to the server.
 * - **in-flight** — sent to the server, awaiting its response.
 *
 * ### Typical usage
 * @code
 * ServerConnection conn;
 * conn.connect("localhost", "4242");
 * conn.handshake("myTeam");
 *
 * CommandQueue queue(conn);
 * queue.push("Look", [](const std::string &resp) {
 *     // parse the vision returned for the Look command
 * });
 * queue.push("Forward");
 *
 * while (queue.hasWork()) {
 *     queue.flush();           // send what the 10-slot limit allows
 *     queue.processResponse(); // read one answer and run its callback
 * }
 * @endcode
 *
 * @note CommandQueue does not own the ServerConnection; the connection must
 *       outlive the queue.
 */
class CommandQueue
{
public:
    /// @brief Callback invoked with the raw server response for a command.
    using ResponseCallback = std::function<void(const std::string &)>;

    /**
     * @brief Construct a queue bound to an existing connection.
     * @param connection  Connection used to send commands and read responses.
     *                    Must outlive this queue.
     */
    explicit CommandQueue(ServerConnection &connection);

    CommandQueue(const CommandQueue &) = delete;
    CommandQueue &operator=(const CommandQueue &) = delete;

    /**
     * @brief Enqueue a command to be sent to the server.
     *
     * The command is appended to the local queue; it is only handed to the
     * server later, when flush() finds a free slot. Order of submission is
     * preserved end-to-end.
     *
     * @param command   Command without trailing newline (e.g. "Forward").
     * @param callback  Optional callback run with the matching response.
     */
    void push(const std::string &command, ResponseCallback callback = nullptr);

    /**
     * @brief Hand as many queued commands to the server as the limit allows.
     *
     * Sends from the front of the queued list while the server still has a free
     * slot (ServerConnection::sendCommand returns true), moving each sent
     * command to the in-flight list.
     *
     * @return Number of commands sent during this call.
     * @throws SocketException / SocketClosed on network errors.
     */
    std::size_t flush();

    /**
     * @brief Read one command response and dispatch it to its callback.
     *
     * Blocks until a response line is available (unsolicited server pushes are
     * routed to the connection's event queue by ServerConnection). The response
     * is matched to the oldest in-flight command (FIFO) and its callback, if
     * any, is invoked.
     *
     * @return true if a response was processed, false if nothing was in flight.
     * @throws SocketException / SocketClosed on network errors.
     */
    bool processResponse();

    /**
     * @brief Convenience step: flush(), then process one pending response.
     *
     * Intended to be called once per game-loop iteration.
     *
     * @throws SocketException / SocketClosed on network errors.
     */
    void update();

    /**
     * @brief Discard every queued and in-flight command.
     *
     * Does not touch the underlying connection's pending-command counter; use
     * after a disconnect/reset where in-flight responses will never arrive.
     */
    void clear();

    /**
     * @brief Whether there is anything left to do.
     * @return true if at least one command is queued or in flight.
     */
    bool hasWork() const noexcept { return !_queued.empty() || !_inFlight.empty(); }

    /// @brief Number of commands waiting to be sent.
    std::size_t queuedCount() const noexcept { return _queued.size(); }

    /// @brief Number of commands sent and awaiting a response.
    std::size_t inFlightCount() const noexcept { return _inFlight.size(); }

private:
    /**
     * @struct PendingCommand
     * @brief A command plus the callback to run on its response.
     */
    struct PendingCommand
    {
        std::string _text;          ///< Command text without trailing newline.
        ResponseCallback _callback; ///< Callback for the response (may be null).
    };

    ServerConnection &_connection;        ///< Borrowed connection (not owned).
    std::deque<PendingCommand> _queued;   ///< Not yet sent to the server.
    std::deque<PendingCommand> _inFlight; ///< Sent, awaiting a response (FIFO).
};

} // namespace ai
