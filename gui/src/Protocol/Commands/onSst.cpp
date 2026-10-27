/*
 * EPITECH PROJECT, 2026
 * onSst.cpp
 * File description:
 * sst command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle sst: the server frequency was changed.
 *
 * @param cmd args: T (new time unit / frequency)
 */
void Handler::onSst(const Command &cmd)
{
    if (cmd._args.size() != 1) {
        Log::error() << "Invalid SST command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    try {
        _gameState._freq = std::stoul(cmd._args[0]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid SST command: argument must be an unsigned integer\n";
    }
    Log::info() << "Server frequency changed to " << _gameState._freq << "\n";
}
