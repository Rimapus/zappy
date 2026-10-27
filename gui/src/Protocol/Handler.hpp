/*
 * EPITECH PROJECT, 2026
 * Handler.hpp
 * File description:
 * handler
 */

#pragma once

#include "../ECS/Coordinator/Coordinator.hpp"
#include "../Game/GameState.hpp"
#include "Commands.hpp"
#include <string>
#include <unordered_map>

/* @brief Bridges incoming protocol commands to ECS state */
class Handler
{
public:
    /**
     * @brief Construct a Handler.
     *
     * @param coordinator The ECS coordinator (must already hold a GameState resource)
     */
    Handler(Coordinator &coordinator);

    /**
     * @brief Dispatch a parsed command to the appropriate handler.
     *
     * @param cmd The parsed command
     */
    void handle(const Command &cmd);

private:
    Coordinator &_coordinator;
    GameState &_gameState;
    std::unordered_map<CommandType, void (Handler::*)(const Command &)> _handlers;

    // tmp
    static void printCmd(const std::string &name, const Command &cmd);

    /**
     * @brief Handle msz: map dimensions.
     *
     * @param cmd args: X (width), Y (height)
     */
    void onMsz(const Command &cmd);

    /**
     * @brief Handle bct: content of a tile.
     *
     * @param cmd args: X, Y, q0..q6 (resource quantities)
     */
    void onBct(const Command &cmd);

    /**
     * @brief Handle tna: register a team.
     *
     * @param cmd args: N (team name)
     */
    void onTna(const Command &cmd);

    /**
     * @brief Handle pnw: connection of a new player.
     *
     * @param cmd args: #n, X, Y, O (orientation 1-4), L (level), N (team)
     */
    void onPnw(const Command &cmd);

    /**
     * @brief Handle ppo: player's position.
     *
     * @param cmd args: #n, X, Y, O (orientation 1-4)
     */
    void onPpo(const Command &cmd);

    /**
     * @brief Handle plv: player's level.
     *
     * @param cmd args: #n, L (level 1-8)
     */
    void onPlv(const Command &cmd);

    /**
     * @brief Handle pin: player's inventory.
     *
     * @param cmd args: #n, X, Y, q0..q6 (resource quantities)
     */
    void onPin(const Command &cmd);

    /**
     * @brief Handle pex: player expulsion.
     *
     * @param cmd args: #n (expelling player id)
     */
    void onPex(const Command &cmd);

    /**
     * @brief Handle pbc: player broadcast a message.
     *
     * @param cmd args: #n, M (message text)
     */
    void onPbc(const Command &cmd);

    /**
     * @brief Handle pic: start of an incantation (by first player).
     *
     * @param cmd args: X, Y, L (required level), #n... (participating players)
     */
    void onPic(const Command &cmd);

    /**
     * @brief Handle pie: end of an incantation.
     *
     * @param cmd args: X, Y, R (result: 0=failure, 1=success)
     */
    void onPie(const Command &cmd);

    /**
     * @brief Handle pfk: player laying an egg.
     *
     * @param cmd args: #n (player id)
     */
    void onPfk(const Command &cmd);

    /**
     * @brief Handle pdr:ressource dropping.
     *
     * @param cmd args: #n, i (resource index 0-6)
     */
    void onPdr(const Command &cmd);

    /**
     * @brief Handle pgt: ressource collecting.
     *
     * @param cmd args: #n, i (resource index 0-6)
     */
    void onPgt(const Command &cmd);

    /**
     * @brief Handle pdi: death of a player.
     *
     * @param cmd args: #n (player id)
     */
    void onPdi(const Command &cmd);

    /**
     * @brief Handle enw: egg laid by a player.
     *
     * @param cmd args: #e (egg id), #n (laying player id), X, Y
     */
    void onEnw(const Command &cmd);

    /**
     * @brief Handle ebo: player connection from an egg.
     *
     * @param cmd args: #e (egg id)
     */
    void onEbo(const Command &cmd);

    /**
     * @brief Handle edi: death of an egg.
     *
     * @param cmd args: #e (egg id)
     */
    void onEdi(const Command &cmd);

    /**
     * @brief Handle sgt: current server frequency.
     *
     * @param cmd args: T (frequency)
     */
    void onSgt(const Command &cmd);

    /**
     * @brief Handle sst: server frequency changed.
     *
     * @param cmd args: T (new frequency)
     */
    void onSst(const Command &cmd);

    /**
     * @brief Handle seg: game is over.
     *
     * @param cmd args: N (winning team name)
     */
    void onSeg(const Command &cmd);

    /**
     * @brief Handle smg: message from the server.
     *
     * @param cmd args: M (message text)
     */
    void onSmg(const Command &cmd);

    /**
     * @brief Handle suc: unkown command.
     *
     * @param cmd unused
     */
    void onSuc(const Command &cmd);

    /**
     * @brief Handle sbp: bad command parameters.
     *
     * @param cmd unused
     */
    void onSbp(const Command &cmd);
};
