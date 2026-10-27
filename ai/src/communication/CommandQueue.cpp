/*
** EPITECH PROJECT, 2026
** CommandQueue.cpp
** File description:
** Client-side command queue for the Zappy AI
*/

#include "CommandQueue.hpp"

#include <utility>

namespace ai {

CommandQueue::CommandQueue(ServerConnection &connection) : _connection(connection)
{
}

void CommandQueue::push(const std::string &command, ResponseCallback callback)
{
    _queued.push_back({command, std::move(callback)});
}

std::size_t CommandQueue::flush()
{
    std::size_t sent = 0;

    while (!_queued.empty()) {
        PendingCommand &front = _queued.front();
        if (!_connection.sendCommand(front._text)) {
            break;
        }
        _inFlight.push_back(std::move(front));
        _queued.pop_front();
        ++sent;
    }
    return sent;
}

bool CommandQueue::processResponse()
{
    if (_inFlight.empty()) {
        return false;
    }

    std::string response = _connection.readResponse();
    PendingCommand command = std::move(_inFlight.front());
    _inFlight.pop_front();

    if (command._callback) {
        command._callback(response);
    }
    return true;
}

void CommandQueue::update()
{
    flush();
    processResponse();
}

void CommandQueue::clear()
{
    _queued.clear();
    _inFlight.clear();
}

} // namespace ai
