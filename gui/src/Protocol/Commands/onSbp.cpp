/*
 * EPITECH PROJECT, 2026
 * onSbp.cpp
 * File description:
 * sbp command handler
 */

#include "../../Log.hpp"
#include "../Handler.hpp"

/**
 * @brief Handle sbp: the last GUI command had bad parameters.
 *
 * @param cmd unused
 */
void Handler::onSbp(const Command &cmd)
{
    (void)cmd;
    Log::warn() << "Server rejected the last command due to bad parameters\n";
}
