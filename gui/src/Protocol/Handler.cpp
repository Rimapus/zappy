/*
 * EPITECH PROJECT, 2026
 * Handler.cpp
 * File description:
 * handler
 */

#include "Handler.hpp"
#include "../Log.hpp"

/**
 * @brief Construct a Handler.
 *
 * @param coordinator The ECS coordinator (must already hold a GameState resource)
 */
Handler::Handler(Coordinator &coordinator)
    : _coordinator(coordinator), _gameState(coordinator.getResource<GameState>()), _handlers({
                                                                                       {CommandType::MSZ, &Handler::onMsz},
                                                                                       {CommandType::BCT, &Handler::onBct},
                                                                                       {CommandType::TNA, &Handler::onTna},
                                                                                       {CommandType::PNW, &Handler::onPnw},
                                                                                       {CommandType::PPO, &Handler::onPpo},
                                                                                       {CommandType::PLV, &Handler::onPlv},
                                                                                       {CommandType::PIN, &Handler::onPin},
                                                                                       {CommandType::PEX, &Handler::onPex},
                                                                                       {CommandType::PBC, &Handler::onPbc},
                                                                                       {CommandType::PIC, &Handler::onPic},
                                                                                       {CommandType::PIE, &Handler::onPie},
                                                                                       {CommandType::PFK, &Handler::onPfk},
                                                                                       {CommandType::PDR, &Handler::onPdr},
                                                                                       {CommandType::PGT, &Handler::onPgt},
                                                                                       {CommandType::PDI, &Handler::onPdi},
                                                                                       {CommandType::ENW, &Handler::onEnw},
                                                                                       {CommandType::EBO, &Handler::onEbo},
                                                                                       {CommandType::EDI, &Handler::onEdi},
                                                                                       {CommandType::SGT, &Handler::onSgt},
                                                                                       {CommandType::SST, &Handler::onSst},
                                                                                       {CommandType::SEG, &Handler::onSeg},
                                                                                       {CommandType::SMG, &Handler::onSmg},
                                                                                       {CommandType::SUC, &Handler::onSuc},
                                                                                       {CommandType::SBP, &Handler::onSbp},
})
{
}

/**
 * @brief Dispatch a parsed command to the appropriate handler.
 *
 * @param cmd The parsed command
 */
void Handler::handle(const Command &cmd)
{
    auto it = _handlers.find(cmd._type);

    if (it != _handlers.end()) {
        (this->*it->second)(cmd);
    }
}

/**
 * @brief Print a command name and its raw arguments to stdout.
 *
 * @param name Human-readable command name
 * @param cmd  The command whose arguments are printed
 */
void Handler::printCmd(const std::string &name, const Command &cmd)
{
    Log::info() << name << " [";
    for (auto const &arg : cmd._args) {
        std::cout << arg << " ";
    }
    std::cout << "]\n";
}
