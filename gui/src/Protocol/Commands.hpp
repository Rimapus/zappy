/*
 * EPITECH PROJECT, 2026
 * Commands.hpp
 * File description:
 * commands
 */

#pragma once

#include <string>
#include <vector>

/* @brief Every command types */
enum class CommandType {
    MSZ,
    BCT,
    MCT,
    TNA,
    PNW,
    PPO,
    PLV,
    PIN,
    PEX,
    PBC,
    PIC,
    PIE,
    PFK,
    PDR,
    PGT,
    PDI,
    ENW,
    EBO,
    EDI,
    SGT,
    SST,
    SEG,
    SMG,
    SUC,
    SBP,
    UNKNOWN
};

/* @brief Command class*/
struct Command
{
    CommandType _type;
    std::vector<std::string> _args;
};
