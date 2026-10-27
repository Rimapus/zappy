/*
** EPITECH PROJECT, 2026
** ServerConnection.cpp
** File description:
** Zappy AI/server protocol layer
*/

#include "ServerConnection.hpp"

#include <cstdio>
#include <stdexcept>
#include <vector>

namespace ai {

// Connection

void ServerConnection::connect(const std::string &host, const std::string &port)
{
    _socket.connect(host, port);
}

void ServerConnection::disconnect()
{
    _socket.disconnect();
    _buffer.clear();
    _pendingCommands = 0;
    _eventQueue.clear();
}

// Handshake

HandshakeInfo ServerConnection::handshake(const std::string &teamName)
{
    // The server sends WELCOME as soon as the TCP connection is established.
    std::string welcome = readLine();
    if (welcome != "WELCOME") {
        throw std::runtime_error("handshake: expected WELCOME, got: " + welcome);
    }

    _socket.send(teamName + "\n");

    std::string clientNumLine = readLine();
    std::string mapLine = readLine();

    int clientNum = std::stoi(clientNumLine);
    int x = 0;
    int y = 0;
    if (std::sscanf(mapLine.c_str(), "%d %d", &x, &y) != 2) {
        throw std::runtime_error("handshake: invalid map dimensions: " + mapLine);
    }

    return {clientNum, x, y}; // aggregate-init: _clientNum, _mapWidth, _mapHeight
}

// ─── Commands ────────────────────────────────────────────────────────────────

bool ServerConnection::sendCommand(const std::string &command)
{
    if (_pendingCommands >= MAX_PENDING_COMMANDS) {
        return false;
    }
    _socket.send(command + "\n");
    ++_pendingCommands;
    return true;
}

std::string ServerConnection::readResponse()
{
    while (true) {
        std::string line = readLine();
        ServerEvent event;
        if (isServerPush(line, event)) {
            if (event._type == ServerEvent::Type::Elevation) {
                if (event._direction < 0) {
                    _armRitualOutcome = true; // "Elevation underway": arm on return
                } else {
                    _expectRitualOutcome = false; // "Current level": success consumed
                    _armRitualOutcome = false;
                }
            }
            _eventQueue.push_back(event);
            continue;
        }
        // A ritual we were dragged into ends with an unsolicited "ko" on failure:
        // it is the ritual's verdict, not a reply to anything we sent. Swallow it.
        if (_expectRitualOutcome && line == "ko") {
            _expectRitualOutcome = false;
            continue;
        }
        // Genuine command reply. If it is the one interrupted by a freeze, only
        // now arm the outcome filter, so this reply itself is never swallowed.
        if (_armRitualOutcome) {
            _expectRitualOutcome = true;
            _armRitualOutcome = false;
        }
        if (_pendingCommands > 0) {
            --_pendingCommands;
        }
        return line;
    }
}

std::string ServerConnection::requestIncantation()
{
    sendCommand("Incantation");
    while (true) {
        std::string line = readLine();
        // The initiator's own outcome: "Elevation underway" (started) or "ko".
        if (line == "Elevation underway") {
            if (_pendingCommands > 0) {
                --_pendingCommands;
            }
            // The initiator now freezes too; its next read is the ritual's
            // trailing "Current level"/"ko", which must be swallowed.
            _expectRitualOutcome = true;
            return "ok";
        }
        if (line == "ko") {
            if (_pendingCommands > 0) {
                --_pendingCommands;
            }
            return "ko";
        }
        // Anything else (broadcasts, eject, dead, the trailing "Current level: N",
        // or another player's elevation push) is unsolicited: queue and keep
        // waiting for our own verdict.
        ServerEvent event;
        if (isServerPush(line, event)) {
            _eventQueue.push_back(event);
            continue;
        }
        if (_pendingCommands > 0) {
            --_pendingCommands;
        }
        return line; // unexpected, but never desynchronise the stream
    }
}

std::string ServerConnection::requestData(const std::string &command)
{
    sendCommand(command);
    while (true) {
        std::string line = readLine();
        ServerEvent event;
        if (isServerPush(line, event)) {
            if (event._type == ServerEvent::Type::Elevation) {
                _expectRitualOutcome = false;
                _armRitualOutcome = false;
            }
            _eventQueue.push_back(event);
            continue;
        }
        // A data query never answers "ok"/"ko": such a line is a stray action
        // reply or a freeze's ritual verdict - swallow it rather than desync.
        if (line == "ok" || line == "ko") {
            _expectRitualOutcome = false;
            continue;
        }
        if (_pendingCommands > 0) {
            --_pendingCommands;
        }
        return line;
    }
}

bool ServerConnection::pollEvent(ServerEvent &event)
{
    if (_eventQueue.empty()) {
        return false;
    }
    event = _eventQueue.front();
    _eventQueue.erase(_eventQueue.begin());
    return true;
}

// ─── Internal ────────────────────────────────────────────────────────────────

std::string ServerConnection::readLine()
{
    while (true) {
        auto pos = _buffer.find('\n');
        if (pos != std::string::npos) {
            std::string line = _buffer.substr(0, pos);
            _buffer.erase(0, pos + 1);
            // Strip optional carriage return for \r\n line endings.
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            return line;
        }
        std::string chunk = _socket.receive(4096);
        _buffer += chunk;
    }
}

bool ServerConnection::isServerPush(const std::string &line, ServerEvent &event) const
{
    event._raw = line;

    // dead — player starved
    if (line == "dead") {
        event._type = ServerEvent::Type::Dead;
        return true;
    }

    // Elevation pushes: "Elevation underway" and "Current level: N" reach every
    // ritual participant unsolicited, so they must never be mistaken for a
    // command response. The initiator's own copy is consumed by requestIncantation.
    if (line == "Elevation underway") {
        event._type = ServerEvent::Type::Elevation;
        event._direction = -1;
        return true;
    }
    int level = 0;
    if (std::sscanf(line.c_str(), "Current level: %d", &level) == 1) {
        event._type = ServerEvent::Type::Elevation;
        event._direction = level;
        return true;
    }

    int k = 0;
    char text[1024]{};

    // message K, text — broadcast from another player
    if (std::sscanf(line.c_str(), "message %d, %1023[^\n]", &k, text) == 2) {
        event._type = ServerEvent::Type::Broadcast;
        event._direction = k;
        event._message = text;
        return true;
    }

    // eject: K — this player was pushed off a tile
    if (std::sscanf(line.c_str(), "eject: %d", &k) == 1) {
        event._type = ServerEvent::Type::Eject;
        event._direction = k;
        return true;
    }

    return false;
}

} // namespace ai
