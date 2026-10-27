/*
 * EPITECH PROJECT, 2026
 * onSmg.cpp
 * File description:
 * smg command handler
 */

#include "../../Constants.hpp"
#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle smg: a free-form message from the server.
 *
 * @param cmd args: M (message text)
 */
void Handler::onSmg(const Command &cmd)
{
    if (cmd._args.empty()) {
        Log::error() << "Invalid SMG command: expected at least 1 argument\n";
        return;
    }
    std::string message = cmd._args[0];
    for (std::size_t i = 1; i < cmd._args.size(); ++i) {
        message += ' ' + cmd._args[i];
    }
    _gameState._chat.push_back({0, "", message, 0.0f, true});
    if (_gameState._chat.size() > CHAT_MAX_MESSAGES) {
        _gameState._chat.pop_front();
    }
    Log::server() << "Server message: " << message << "\n";
}
