/*
** EPITECH PROJECT, 2026
** IPolicy.hpp
** File description:
** Decision-policy interface - the single seam between the game loop and the
** strategy that chooses actions (RL model, scripted, ...)
*/

#pragma once

#include "observation/Observation.hpp"

#include <string>

namespace ai {

/**
 * @class IPolicy
 * @brief Strategy that turns an Observation into the next action.
 *
 * This is the one extension point of the AI: ZappyAI builds an Observation each
 * decision step and asks the active policy for a command. Swapping the brain
 * (RL model <-> scripted <-> hybrid) is just constructing a different IPolicy -
 * the game loop never changes. See makePolicy() in Policy.hpp.
 */
class IPolicy
{
public:
    virtual ~IPolicy() = default;

    /**
     * @brief Choose the next action for the current observation.
     * @param observation  Snapshot of the game state for this step.
     * @return The protocol command to send (without trailing newline), e.g.
     *         "Forward", "Take linemate", "Incantation". An empty string means
     *         "do nothing this step".
     */
    virtual std::string decide(const Observation &observation) = 0;
};

} // namespace ai
