/*
 * EPITECH PROJECT, 2026
 * onSgt.cpp
 * File description:
 * sgt command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle sgt: receive the current server frequency.
 *
 * @param cmd args: T (time unit / frequency)
 */
void Handler::onSgt(const Command &cmd)
{
    if (cmd._args.size() != 1) {
        Log::error() << "Invalid SGT command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    try {
        _gameState._freq = std::stoul(cmd._args[0]);
    } catch (const std::exception &e) {
        Log::error() << "Invalid SGT command: argument must be an unsigned integer\n";
    }
    Log::info() << "Server frequency set to " << _gameState._freq << "\n";
}
