/*
 * EPITECH PROJECT, 2026
 * onSuc.cpp
 * File description:
 * suc command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle suc: the server did not recognise the last GUI command.
 *
 * @param cmd unused
 */
void Handler::onSuc(const Command &cmd)
{
    (void)cmd;
    Log::warn() << "Server did not understand the last command\n";
}
