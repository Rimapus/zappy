/*
** EPITECH PROJECT, 2026
** Policy.cpp
** File description:
** Factory and selector for the decision policy
*/

#include "policy/Policy.hpp"

#include "policy/ModelPolicy.hpp"
#include "policy/ScriptedPolicy.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace ai {

std::unique_ptr<IPolicy> makePolicy(PolicyKind kind, const std::vector<std::string> &agentCommand)
{
    switch (kind) {
    case PolicyKind::Model:
        return std::make_unique<ModelPolicy>(agentCommand);
    case PolicyKind::Scripted:
        // Hand-written brain: needs no RL agent process, so agentCommand is unused.
        (void)agentCommand;
        return std::make_unique<ScriptedPolicy>();
    }
    throw std::runtime_error("unknown policy kind");
}

PolicyKind policyKindFromString(const std::string &name)
{
    std::string lowered = name;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lowered == "scripted") {
        return PolicyKind::Scripted;
    }
    return PolicyKind::Model;
}

} // namespace ai
