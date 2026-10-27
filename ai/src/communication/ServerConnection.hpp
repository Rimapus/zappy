/*
** EPITECH PROJECT, 2026
** ServerConnection.hpp
** File description:
** Zappy AI/server protocol layer
*/

#pragma once

#include "Socket.hpp"

#include <string>
#include <vector>

namespace ai {

/**
 * @struct HandshakeInfo
 * @brief Data returned by the connection handshake.
 *
 * After a successful handshake the server communicates:
 * - the number of remaining slots for the team,
 * - the dimensions of the game map.
 */
struct HandshakeInfo
{
    int _clientNum; ///< Number of available slots for the team on the server.
    int _mapWidth;  ///< Width  (X) of the game map in tiles.
    int _mapHeight; ///< Height (Y) of the game map in tiles.
};

/**
 * @struct ServerEvent
 * @brief Unsolicited message pushed by the server at any time.
 *
 * The server can push three kinds of events independently of the command
 * queue:
 * - **Broadcast** — another player used the `Broadcast` command.
 * - **Eject**     — this player has been pushed off a tile.
 * - **Dead**      — this player has run out of food and died.
 *
 * These events are separated from command responses by ServerConnection and
 * stored in an internal queue; the game loop retrieves them via pollEvent().
 */
struct ServerEvent
{
    /**
     * @enum Type
     * @brief Category of the server-pushed event.
     */
    enum class Type {
        Broadcast,
        Eject,
        Dead,
        Elevation,
        Unknown
    };

    Type _type;           ///< Category of the event.
    std::string _raw;     ///< Original line as received from the server.
    int _direction{0};    ///< Tile direction (Broadcast/Eject), or new level
                          ///< (Elevation "Current level: N"), -1 if none.
    std::string _message; ///< Text payload (for Broadcast only).
};

/**
 * @class ServerConnection
 * @brief Zappy AI/server protocol layer built on top of Socket.
 *
 * Handles:
 * - The connection handshake (WELCOME → team name → CLIENT-NUM + map size).
 * - Sending commands with respect to the 10-command queue limit.
 * - Reading command responses while transparently routing unsolicited server
 *   events (broadcast, eject, dead) to a separate event queue.
 *
 * ### Typical usage
 * @code
 * ServerConnection conn;
 * conn.connect("localhost", "4242");
 * auto info = conn.handshake("myTeam");
 *
 * conn.sendCommand("Look");
 * std::string response = conn.readResponse(); // "[player, food, ...]"
 *
 * ServerEvent ev;
 * while (conn.pollEvent(ev)) {
 *     // handle broadcast / eject / dead
 * }
 * @endcode
 */
class ServerConnection
{
public:
    ServerConnection() = default;
    ~ServerConnection() = default;

    ServerConnection(const ServerConnection &) = delete;
    ServerConnection &operator=(const ServerConnection &) = delete;

    /**
     * @brief Open a TCP connection to the Zappy server.
     * @param host  Hostname or IP address.
     * @param port  Port number as a string.
     * @throws SocketException on connection failure.
     */
    void connect(const std::string &host, const std::string &port);

    /**
     * @brief Close the connection and reset internal state.
     *
     * Clears the read buffer, resets the pending-command counter, and
     * discards any queued events.  Safe to call on an already-disconnected
     * object.
     */
    void disconnect();

    /**
     * @brief Perform the Zappy connection handshake.
     *
     * Expected exchange (subject §4):
     * @code
     * <- WELCOME\n
     * -> teamName\n
     * <- CLIENT-NUM\n
     * <- X Y\n
     * @endcode
     *
     * @param teamName  Name of the team to join.
     * @return HandshakeInfo containing slot count and map dimensions.
     * @throws std::runtime_error if the server does not send WELCOME first,
     *         or if the map dimension line is malformed.
     * @throws SocketException / SocketClosed on network errors.
     */
    HandshakeInfo handshake(const std::string &teamName);

    /**
     * @brief Send a command to the server.
     *
     * Appends `\n` automatically.  The server accepts at most 10 commands in
     * flight at a time; excess commands are silently dropped by the server.
     *
     * @param command  Command string without trailing newline (e.g. "Forward").
     * @return true if the command was sent, false if the queue is already full.
     * @throws SocketException / SocketClosed on network errors.
     */
    bool sendCommand(const std::string &command);

    /**
     * @brief Block until a command response is received.
     *
     * Lines that are unsolicited server pushes (broadcast, eject, dead) are
     * silently moved to the internal event queue and do **not** affect the
     * pending-command counter.
     *
     * @return The raw response line (e.g. "ok", "ko", "[player, food, ...]").
     * @throws SocketException / SocketClosed on network errors.
     */
    std::string readResponse();

    /**
     * @brief Send "Incantation" and read only its own outcome.
     *
     * Elevation is special: on success the server pushes "Elevation underway" and,
     * 300/f later, "Current level: N" - and it pushes those same two lines
     * unsolicited to every *other* participant of a group ritual. Treating them as
     * ordinary responses would desynchronise the request/response stream of any
     * drone dragged into someone else's incantation. So those lines are always
     * routed to the event queue by readResponse(); this method is the one place
     * that consumes the initiator's own "Elevation underway"/"ko" as a response.
     *
     * @return "ok" if the ritual started ("Elevation underway"), "ko" on refusal.
     * @throws SocketException / SocketClosed on network errors.
     */
    std::string requestIncantation();

    /**
     * @brief Send a data query (Inventory/Look/Connect_nbr) and read its reply,
     *        skipping any stray status line.
     *
     * These queries always answer with a bracketed list or a number - never
     * "ok"/"ko". So any "ok"/"ko" that turns up here is a misplaced action reply
     * or a failed-ritual verdict leaking in from a freeze, and is discarded. This
     * makes perception self-healing: a desynchronising stray line is absorbed
     * instead of being parsed as food=0 / empty vision.
     *
     * @param command  One of "Inventory", "Look", "Connect_nbr".
     * @return The genuine data reply line.
     */
    std::string requestData(const std::string &command);

    /**
     * @brief Pop one unsolicited server event from the queue.
     *
     * Non-blocking: returns immediately even if the queue is empty.
     *
     * @param[out] event  Filled with the next event if one is available.
     * @return true if an event was dequeued, false if the queue was empty.
     */
    bool pollEvent(ServerEvent &event);

    /**
     * @brief Return whether the underlying socket is open.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const noexcept { return _socket.isConnected(); }

    /// Maximum number of commands that may be in-flight simultaneously.
    static constexpr int MAX_PENDING_COMMANDS = 10;

private:
    /**
     * @brief Read one `\n`-terminated line from the server, using an internal
     *        buffer to handle partial recv() results.
     * @return Line content without the trailing newline (or `\r\n`).
     */
    std::string readLine();

    /**
     * @brief Check whether @p line is an unsolicited server push.
     *
     * Recognises:
     * - `message K, text` → ServerEvent::Type::Broadcast
     * - `eject: K`        → ServerEvent::Type::Eject
     * - `dead`            → ServerEvent::Type::Dead
     *
     * @param[in]  line   Line to inspect.
     * @param[out] event  Populated if the line is a server push.
     * @return true if @p line is a server push and @p event was filled.
     */
    bool isServerPush(const std::string &line, ServerEvent &event) const;

    Socket _socket;
    std::string _buffer;
    int _pendingCommands{0};

    /// Set once a ritual's "Elevation underway" has been seen *and* the in-flight
    /// command it interrupted has been answered: the very next response read is
    /// then the ritual's own terminal line ("ko" on failure, or "Current level"
    /// which is filtered anyway), which must be swallowed, not mistaken for a reply.
    bool _expectRitualOutcome{false};
    /// "Elevation underway" was seen this read; arm @ref _expectRitualOutcome only
    /// after the current (interrupted) command's real reply has been returned.
    bool _armRitualOutcome{false};

    std::vector<ServerEvent> _eventQueue;
};

} // namespace ai
