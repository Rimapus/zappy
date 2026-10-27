/*
** EPITECH PROJECT, 2026
** Policy.hpp
** File description:
** Factory and selector for the decision policy - the switch point between the
** RL model and a future scripted brain
*/

#pragma once

#include "policy/IPolicy.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ai {

/**
 * @enum PolicyKind
 * @brief Which decision brain ZappyAI should drive the drone with.
 *
 * Add a new strategy by adding an entry here and a case in makePolicy() - the
 * game loop in ZappyAI is agnostic and needs no change.
 */
enum class PolicyKind {
    Model,    ///< Trained RL models, one per level (ModelPolicy). Implemented.
    Scripted, ///< Hand-written heuristics. Not implemented yet (see makePolicy).
};

/**
 * @brief Build the requested decision policy.
 * @param kind          Which brain to construct.
 * @param agentCommand  argv of the RL agent process (used by PolicyKind::Model).
 * @return An owned IPolicy ready to decide().
 * @throws std::runtime_error if @p kind is not implemented yet (Scripted).
 * @throws ProcessException   if the Model agent process fails to start.
 */
std::unique_ptr<IPolicy> makePolicy(PolicyKind kind, const std::vector<std::string> &agentCommand);

/**
 * @brief Parse a policy kind from a string (e.g. the $ZAPPY_POLICY env var).
 * @param name  "model" or "scripted" (case-insensitive); anything else -> Model.
 */
PolicyKind policyKindFromString(const std::string &name);

} // namespace ai
