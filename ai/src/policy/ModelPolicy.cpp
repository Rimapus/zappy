/*
** EPITECH PROJECT, 2026
** ModelPolicy.cpp
** File description:
** Decision policy backed by the trained RL models (one model per level)
*/

#include "policy/ModelPolicy.hpp"

#include "observation/Serializer.hpp"

namespace ai {

ModelPolicy::ModelPolicy(const std::vector<std::string> &agentCommand)
{
    _bridge.start(agentCommand);
}

std::string ModelPolicy::decide(const Observation &observation)
{
    std::string line = serialize(observation);
    if (!line.empty() && line.back() == '\n') {
        line.pop_back();
    }
    _bridge.send(line);
    return _bridge.readLine();
}

} // namespace ai
