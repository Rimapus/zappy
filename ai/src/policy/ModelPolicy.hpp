/*
** EPITECH PROJECT, 2026
** ModelPolicy.hpp
** File description:
** Decision policy backed by the trained RL models (one model per level),
** run in a Python child process over the RlBridge
*/

#pragma once

#include "communication/RlBridge.hpp"
#include "policy/IPolicy.hpp"

#include <string>
#include <vector>

namespace ai {

/**
 * @class ModelPolicy
 * @brief Drives the drone with the trained RL policy - one model per level.
 *
 * The observation (which carries the player's level) is serialized to JSON and
 * handed to a Python agent over the RlBridge; the agent selects the model that
 * matches the level, runs it, and replies with the protocol command to send.
 * Keeping the per-level selection and the tensor maths on the Python side means
 * the C++ client stays free of any ML dependency.
 *
 * @code
 * ai::ModelPolicy policy({"python3", "ai/python/rl_agent.py", "ai/models"});
 * std::string command = policy.decide(observation);
 * @endcode
 */
class ModelPolicy : public IPolicy
{
public:
    /**
     * @brief Spawn the Python agent process.
     * @param agentCommand  argv of the agent (e.g. {"python3", "rl_agent.py"}).
     * @throws ProcessException if the process cannot be started.
     */
    explicit ModelPolicy(const std::vector<std::string> &agentCommand);

    /**
     * @brief Send the observation to the agent and return its chosen command.
     * @throws ProcessClosed / ProcessException on bridge I/O failure.
     */
    std::string decide(const Observation &observation) override;

private:
    RlBridge _bridge;
};

} // namespace ai
