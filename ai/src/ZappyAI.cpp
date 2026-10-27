/*
** EPITECH PROJECT, 2026
** ZappyAi.cpp
** File description:
** Zappy AI implementation - perceive / decide (RL model per level) / act loop
*/

#include "ZappyAi.hpp"

#include "communication/RlBridge.hpp"
#include "parsing/Inventory.hpp"
#include "parsing/Look.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace zappy {

namespace {

/// @brief argv of the Python RL agent. Override with $ZAPPY_RL_CMD (space split).
/// The agent reads one JSON observation per line on stdin and writes back the
/// protocol command to run; it selects the model from the observation's level.
///
/// The default locates the agent relative to the current directory so the binary
/// works whether it is launched from the repo root or from ai/.
std::vector<std::string> agent_command()
{
    if (const char *env = std::getenv("ZAPPY_RL_CMD")) {
        std::vector<std::string> command;
        std::istringstream stream(env);
        std::string word;
        while (stream >> word) {
            command.push_back(word);
        }
        if (!command.empty()) {
            return command;
        }
    }
    for (const std::string &root : {std::string("ai/"), std::string()}) {
        const std::string script = root + "python/rl_agent.py";
        if (std::filesystem::exists(script)) {
            return {"python3", script, root + "models"};
        }
    }
    return {"python3", "ai/python/rl_agent.py", "ai/models"};
}

} // namespace

ZappyAI::ZappyAI(const std::string &host, const std::string &port, const std::string &team)
    : _host(host), _port(port), _team(team)
{
    if (const char *env = std::getenv("ZAPPY_POLICY")) {
        _policyKind = ai::policyKindFromString(env);
    }
}

ZappyAI::~ZappyAI()
{
    _conn.disconnect();
}

std::string ZappyAI::request(const std::string &command)
{
    _conn.sendCommand(command);
    return _conn.readResponse();
}

std::vector<ai::Broadcast> ZappyAI::drain_events(bool &died)
{
    std::vector<ai::Broadcast> broadcasts;
    ai::ServerEvent event;

    while (_conn.pollEvent(event)) {
        switch (event._type) {
        case ai::ServerEvent::Type::Dead:
            died = true;
            break;
        case ai::ServerEvent::Type::Broadcast:
            broadcasts.push_back({event._direction, event._message});
            break;
        case ai::ServerEvent::Type::Elevation:
            // "Current level: N" carries the new level (-1 for "Elevation
            // underway"); keep the tracker in step for the model policy.
            if (event._direction > 0 && event._direction != _level) {
                _level = event._direction;
            }
            break;
        default:
            break;
        }
    }
    return broadcasts;
}

ai::Observation ZappyAI::perceive(std::vector<ai::Broadcast> broadcasts)
{
    ai::Observation observation;

    observation._level = _level;
    observation._broadcasts = std::move(broadcasts);
    // requestData skips stray status lines so a freeze-injected "ok"/"ko" can
    // never be parsed as the inventory or vision (the old food=0 / blind desync).
    observation._inventory = ai::parseInventory(_conn.requestData("Inventory"));
    observation._vision = ai::parseLook(_conn.requestData("Look"));
    try {
        observation._freeSlots = std::stoi(_conn.requestData("Connect_nbr"));
    } catch (const std::exception &) {
        observation._freeSlots = 0;
    }
    return observation;
}

void ZappyAI::run()
{
    std::cout << std::unitbuf;
    _conn.connect(_host, _port);

    const ai::HandshakeInfo info = _conn.handshake(_team);
    std::cout << "Connected - slots: " << info._clientNum
              << "  map: " << info._mapWidth << "x" << info._mapHeight << "\n";

    if (!_policy) {
        _policy = ai::makePolicy(_policyKind, agent_command());
    }

    while (_conn.isConnected()) {
        bool died = false;
        std::vector<ai::Broadcast> broadcasts = drain_events(died);
        if (died) {
            std::cout << "Drone died.\n";
            break;
        }

        const ai::Observation observation = perceive(std::move(broadcasts));

        std::string command;
        try {
            command = _policy->decide(observation);
        } catch (const ai::ProcessClosed &) {
            std::cerr << "RL agent closed the pipe - stopping.\n";
            break;
        } catch (const ai::ProcessException &error) {
            std::cerr << "RL agent error (" << error.what() << ") - stopping.\n";
            break;
        }
        if (command.empty()) {
            continue;
        }

        // Incantation needs its own reader: its "Elevation underway"/"Current
        // level" lines are routed to the event queue (they also arrive unsolicited
        // to every other ritual participant), so request() must not await them.
        if (command.rfind("Incantation", 0) == 0) {
            _conn.requestIncantation();
        } else {
            request(command);
        }
    }
}

} // namespace zappy
