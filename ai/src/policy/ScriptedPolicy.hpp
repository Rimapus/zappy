/*
** EPITECH PROJECT, 2026
** ScriptedPolicy.hpp
** File description:
** Hand-written decision policy: a coordinated state machine that survives,
** rallies the whole team onto a single tile (a stationary "conductor"), pools
** stones on a shared altar and incants in lockstep so a cohort of six drones
** rides every elevation up to the maximum level (8).
*/

#pragma once

#include "policy/IPolicy.hpp"
#include "policy/scripted/Requirements.hpp"

#include <string>

namespace ai {

/**
 * @class ScriptedPolicy
 * @brief Rule-based brain that drives one drone toward the top level (8).
 *
 * Every drone is autonomous (it only ever sees its own Observation), yet the
 * team behaves as one because coordination rides the **global** Broadcast
 * channel: every player hears every broadcast, with a bearing pointing the
 * short way to the emitter. The protocol is:
 *
 * - **Elect a conductor** - the lowest-id drone of the whole team. It never
 *   moves: its tile is the single rally / altar for everyone, of every level.
 * - **Home** - followers walk toward the conductor's beacon; a beacon heard
 *   from tile 0 means "you are co-located", the precise arrival signal.
 * - **Stock the altar** - drones drop every stone they carry onto the rally
 *   tile and make short foraging trips to nearby tiles to fetch what it lacks.
 * - **Incant in lockstep** - the server elevates exactly the required number of
 *   same-level players, so the co-located cohort climbs in batches; anyone with
 *   the stones present and enough players on the tile just fires Incantation
 *   (a wrong-quorum `ko` is instant and free), and the whole pile rises until
 *   six drones reach level 8 and the game is won.
 *
 * Survival always preempts everything: drones bank a deep food reserve so they
 * never starve while parked, and crucially never starve mid-incantation (a
 * frozen participant cannot eat for the ritual's whole 300/f).
 *
 * The policy is a pure function of the Observation plus a little carried state
 * (identity, tick, conductor bearing, foraging target); it never blocks.
 */
class ScriptedPolicy : public IPolicy
{
public:
    /// @brief Build the policy and seed the drone's unique broadcast identity.
    ScriptedPolicy();

    /**
     * @brief Choose this step's protocol command from the observation.
     * @param observation  Fresh game-state snapshot for the drone.
     * @return The command to send, or "" to idle (e.g. while holding at rally).
     */
    std::string decide(const Observation &observation) override;

private:
    /// @brief Pick the command for this step, ignoring bookkeeping/logging.
    std::string plan(const Observation &observation, int level);

    /// @brief Post-process a command so turns are always followed by a step.
    std::string commit(std::string command);

    /// @brief Trace the chosen command to stderr when $ZAPPY_DEBUG is set.
    void debug(const Observation &observation, int level, const std::string &command) const;

    /// @brief Whether this drone is the team's conductor (lowest id heard).
    bool isConductor() const { return _conductorId == _id; }

    /// @brief Whether the drone is parked on the rally tile (conductor or a
    ///        follower currently co-located with it).
    bool atRally() const { return isConductor() || _atRally; }

    /// @brief Fold this step's broadcasts into the conductor election + bearing.
    void ingestBroadcasts(const Observation &observation);

    /// @brief Drop-everything food run when starving: eat, chase, or explore.
    std::string seekFood(const Observation &observation) const;

    /// @brief Eat now if food has dropped below @p threshold and food is on the
    ///        tile or visible nearby; "" otherwise. The universal hunger guard.
    std::string maybeEat(const Observation &observation, int threshold);

    /// @brief Parked-at-rally behaviour: stock the altar, then incant or wait.
    std::string runRally(const Observation &observation, int level);

    /// @brief Travel toward the conductor (or run the active foraging trip).
    std::string homeToRally(const Observation &observation);

    /// @brief Walk to / pick up the stone the current forage trip is after.
    std::string forageMove(const Observation &observation);

    /// @brief Survive in place: top up food from the tile, otherwise idle.
    std::string holdAtRally(const Observation &observation) const;

    /// @brief Periodic team beacon, or "" when it is not time to broadcast.
    std::string maybeBeacon();

    /// @brief Best-effort wandering used before any beacon has been heard.
    std::string explore() const;

    int _id{0};    ///< Unique broadcast identity (process id).
    long _tick{0}; ///< Decision counter, drives beacon cadence.
    int _level{1}; ///< Last inferred level.

    int _conductorId{0};    ///< Smallest id heard so far (this drone if alone).
    int _conductorDir{-1};  ///< Last bearing toward the conductor (-1 unknown).
    long _conductorTick{0}; ///< Tick the conductor bearing was last refreshed.
    bool _atRally{false};   ///< Sticky: co-located with the conductor (tile 0).

    bool _foraging{false};                       ///< A stone-fetch excursion is in progress.
    ObjectType _forageType{ObjectType::Unknown}; ///< Stone the trip is after.

    long _beaconTick{-100};  ///< Tick of the last beacon this drone sent.
    int _forks{0};           ///< Number of eggs this drone has laid.
    bool _justTurned{false}; ///< Last step was a turn: step before turning again.
};

} // namespace ai
