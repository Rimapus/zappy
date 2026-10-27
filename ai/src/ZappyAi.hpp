/*
** EPITECH PROJECT, 2026
** ZappyAi.hpp
** File description:
** Zappy AI entrypoint and AI class declaration
*/

#pragma once

#include "communication/ServerConnection.hpp"
#include "observation/Observation.hpp"
#include "policy/IPolicy.hpp"
#include "policy/Policy.hpp"

#include <memory>
#include <string>
#include <vector>

/// @brief Default server port used when -p is not provided.
#define DEFAULT_PORT "4242"

/// @namespace zappy
/// @brief Top-level namespace for the Zappy AI binary.
namespace zappy {

/**
 * @class ZappyAI
 * @brief Top-level controller for the Zappy AI client.
 *
 * Owns the connection to the server and drives the game loop.
 * After construction the parameters can be overridden with the setters before
 * calling run().
 *
 * ### Lifecycle
 * 1. Instantiate with host / port / team (or use the setters).
 * 2. Call run() - blocks until the game ends or the connection is lost.
 * 3. The destructor closes the connection automatically.
 */
class ZappyAI
{
public:
    /**
     * @brief Construct a ZappyAI with connection parameters.
     * @param host  Hostname or IP address of the server.
     * @param port  Port number as a string (default: "4242").
     * @param team  Name of the team to join.
     */
    ZappyAI(const std::string &host, const std::string &port, const std::string &team);

    /// @brief Disconnect and destroy the AI object.
    ~ZappyAI();

    /**
     * @brief Set the server hostname.
     * @param host  Hostname or IP address.
     */
    void set_host(const std::string &host) { _host = host; }

    /**
     * @brief Set the server port.
     * @param port  Port number as a string.
     */
    void set_port(const std::string &port) { _port = port; }

    /**
     * @brief Set the team name.
     * @param team  Team identifier sent during handshake.
     */
    void set_team(const std::string &team) { _team = team; }

    /**
     * @brief Choose which decision brain to use (default: PolicyKind::Scripted).
     *
     * Must be called before run() to take effect. This is the switch between
     * the RL model and a future scripted policy.
     * @param kind  The policy to build lazily when run() starts.
     */
    void set_policy_kind(ai::PolicyKind kind) { _policyKind = kind; }

    /**
     * @brief Inject a ready-made decision policy, bypassing the factory.
     *
     * Lets callers (or tests) plug in any IPolicy - e.g. a mock or a custom
     * brain - without ZappyAI knowing how it was built. Takes ownership.
     * @param policy  The policy to drive the drone with.
     */
    void set_policy(std::unique_ptr<ai::IPolicy> policy) { _policy = std::move(policy); }

    /**
     * @brief Connect to the server and start the AI game loop.
     *
     * Performs the handshake, builds the policy (if none was injected), then
     * loops: perceive -> ask the policy -> execute the command. Blocks until the
     * game ends, the player dies, or the connection is lost.
     *
     * @throws ai::SocketException  on network errors.
     * @throws ai::SocketClosed     when the server closes the connection.
     * @throws std::runtime_error   if the handshake fails or the policy can't be built.
     */
    void run();

private:
    /// @brief Send a command and return its (synchronous) server response.
    std::string request(const std::string &command);

    /// @brief Drain pending server pushes; collect broadcasts, report death.
    /// @param[out] died  Set to true if a Dead event was received.
    /// @return The broadcasts heard since the last call.
    std::vector<ai::Broadcast> drain_events(bool &died);

    /// @brief Build the current observation (inventory, vision, slots, level).
    /// @param broadcasts  Broadcasts heard this step (moved into the observation).
    ai::Observation perceive(std::vector<ai::Broadcast> broadcasts);

    std::string _host;
    std::string _port;
    std::string _team;

    ai::ServerConnection _conn;
    std::unique_ptr<ai::IPolicy> _policy; ///< Active decision brain.
    /// Brain to build in run(). Defaults to the hand-written Scripted brain (it
    /// reliably climbs to level 8), since the subject launches `zappy_ai` with no
    /// flag. Override to the RL models with `ZAPPY_POLICY=model` (still in dev).
    ai::PolicyKind _policyKind{ai::PolicyKind::Scripted};
    int _level{1}; ///< Current elevation level.
};

} // namespace zappy
