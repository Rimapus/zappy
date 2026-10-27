/*
 * EPITECH PROJECT, 2026
 * onPfk.cpp
 * File description:
 * pfk command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle pfk: a player has started laying an egg.
 *
 * @param cmd args: #n (player id)
 */
void Handler::onPfk(const Command &cmd)
{
    if (cmd._args.size() != 1) {
        Log::error() << "Invalid PFK command: expected 1 argument, got " << cmd._args.size() << "\n";
        return;
    }
    Log::info() << "Player " << cmd._args[0] << " has started laying an egg\n";
}
