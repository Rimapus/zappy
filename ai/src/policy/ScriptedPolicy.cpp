/*
** EPITECH PROJECT, 2026
** ScriptedPolicy.cpp
** File description:
** Hand-written coordinated brain - survive, rally on a stationary conductor,
** pool stones on a shared altar and incant in lockstep up to the maximum level.
*/

#include "policy/ScriptedPolicy.hpp"

#include "policy/scripted/Requirements.hpp"
#include "policy/scripted/VisionGeometry.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace ai {

namespace {

constexpr int FOOD_CRITICAL = 8; ///< At/below this, abandon all goals and eat.
constexpr int FOOD_LOW = 16;     ///< Below this, top up from a nearby food tile
                                 ///< even while travelling, so we never starve.
constexpr int FOOD_RESERVE = 28; ///< Bank food up to here while it is reachable:
                                 ///< outlasts long quorum waits AND the 300/f
                                 ///< incantation freeze (no eating while frozen).
constexpr int FOOD_FERRY = 46;   ///< While travelling, bank free food up to here:
                                 ///< the surplus over the reserve is ferried to the
                                 ///< rally so the parked cohort never has to leave.
constexpr int FOOD_BUFFER = 14;  ///< Food a drone tops the rally tile up to, so the
                                 ///< immobile conductor and high-level holders eat
                                 ///< in place instead of starving on the desert tile.
constexpr int FOOD_INCANT = 11;  ///< Only join/start a ritual this well fed, so a
                                 ///< frozen participant survives the whole 300/f
                                 ///< freeze (and the next while the cohort re-stocks).
constexpr int FOOD_FORK = 26;    ///< Only spend 42/f forking when this well fed.
constexpr int MAX_FORKS = 2;     ///< Eggs a single drone lays over its life.

constexpr long BEACON_FAST = 2;      ///< Conductor beacon cadence: keeps a homing
                                     ///< follower's bearing fresh while still leaving
                                     ///< the conductor turns to incant and fork.
constexpr long BEACON_SLOW = 8;      ///< Follower beacon cadence (election upkeep).
constexpr long CONDUCTOR_STALE = 30; ///< Ticks of silence before re-electing self.

constexpr int FORAGE_RADIUS = 2;   ///< Only fetch a missing stone this near, so a
                                   ///< restocking trip is a short hop off the rally.
constexpr long FORAGE_STAGGER = 6; ///< Window length of the rotating forage slice.

/// First tile of the cone, or an empty tile when vision is unavailable.
const Tile &ownTile(const Observation &observation)
{
    static const Tile empty;
    return observation._vision.empty() ? empty : observation._vision.front();
}

/// Whether the tile underfoot already satisfies the level's stone requirement.
bool altarStocked(int level, const Tile &tile)
{
    const LevelRequirement &requirement = requirementFor(level);
    for (std::size_t i = 0; i < STONE_ORDER.size(); i++) {
        if (tile.count(STONE_ORDER[i]) < requirement._stones[i]) {
            return false;
        }
    }
    return true;
}

/// First stone type the altar still lacks for @p level, or Unknown if complete.
ObjectType missingStone(int level, const Tile &tile)
{
    const LevelRequirement &requirement = requirementFor(level);
    for (std::size_t i = 0; i < STONE_ORDER.size(); i++) {
        if (tile.count(STONE_ORDER[i]) < requirement._stones[i]) {
            return STONE_ORDER[i];
        }
    }
    return ObjectType::Unknown;
}

/// Nearest visible tile (excluding own) that holds at least one player, ties
/// broken by proximity. Lets a homing drone lock onto a lone conductor by sight
/// even before any density gradient exists - the fix for orbiting forever.
int nearestPlayerTile(const std::vector<Tile> &vision)
{
    int best = -1;
    TileOffset bestOffset{};
    for (int index = 1; index < static_cast<int>(vision.size()); index++) {
        if (vision[static_cast<std::size_t>(index)].count(ObjectType::Player) == 0) {
            continue;
        }
        const TileOffset offset = offsetForIndex(index);
        if (best == -1 || offset._forward < bestOffset._forward ||
            (offset._forward == bestOffset._forward &&
             std::abs(offset._lateral) < std::abs(bestOffset._lateral))) {
            best = index;
            bestOffset = offset;
        }
    }
    return best;
}

/// Nearest tile holding @p type strictly ahead of the drone (skips own tile,
/// which is the altar - taking from it would defeat the stockpiling).
int nearestNeighbour(const std::vector<Tile> &vision, ObjectType type)
{
    int best = -1;
    TileOffset bestOffset{};
    for (int index = 1; index < static_cast<int>(vision.size()); index++) {
        if (!vision[static_cast<std::size_t>(index)].contains(type)) {
            continue;
        }
        const TileOffset offset = offsetForIndex(index);
        if (best == -1 || offset._forward < bestOffset._forward ||
            (offset._forward == bestOffset._forward &&
             std::abs(offset._lateral) < std::abs(bestOffset._lateral))) {
            best = index;
            bestOffset = offset;
        }
    }
    return best;
}

} // namespace

ScriptedPolicy::ScriptedPolicy() : _id(static_cast<int>(::getpid()))
{
    _conductorId = _id;
}

void ScriptedPolicy::ingestBroadcasts(const Observation &observation)
{
    // A conductor that has gone silent for too long is presumed dead: reclaim
    // the centre so the team can re-converge around whoever is now the lowest id.
    if (!isConductor() && _tick - _conductorTick > CONDUCTOR_STALE) {
        _conductorId = _id;
        _conductorDir = -1;
        _atRally = false;
    }
    for (const Broadcast &broadcast : observation._broadcasts) {
        std::istringstream stream(broadcast._message);
        std::string tag;
        int heardId = 0;
        if (!(stream >> tag >> heardId) || tag != "Z") {
            continue;
        }
        // The whole team rallies to one conductor - the global lowest id - so
        // every level's quorum forms on a single tile. A bearing of 0 means the
        // emitter is on our tile: that is the precise "arrived at rally" signal.
        if (heardId < _conductorId) {
            _conductorId = heardId;
            _conductorDir = broadcast._direction;
            _conductorTick = _tick;
            _atRally = broadcast._direction == 0;
        } else if (heardId == _conductorId && !isConductor()) {
            _conductorDir = broadcast._direction;
            _conductorTick = _tick;
            _atRally = broadcast._direction == 0;
        }
    }
}

std::string ScriptedPolicy::seekFood(const Observation &observation) const
{
    if (ownTile(observation).contains(ObjectType::Food)) {
        return "Take food";
    }
    const int index = nearestTile(observation._vision, ObjectType::Food);
    if (index > 0) {
        return stepToward(index);
    }
    return explore();
}

std::string ScriptedPolicy::maybeEat(const Observation &observation, int threshold)
{
    if (observation._inventory._food >= threshold) {
        return "";
    }
    if (ownTile(observation).contains(ObjectType::Food)) {
        return "Take food";
    }
    const int index = nearestTile(observation._vision, ObjectType::Food);
    if (index > 0) {
        // Stepping off to graze: a follower must drop its "arrived" flag so it
        // homes back afterwards; the conductor simply drifts toward the food.
        if (_atRally && !isConductor()) {
            _atRally = false;
        }
        return stepToward(index);
    }
    return ""; // no food in reach: let the caller's normal goal carry on
}

std::string ScriptedPolicy::holdAtRally(const Observation &observation) const
{
    // Parked and waiting: bank food while it is free underfoot, otherwise idle
    // (still perceiving, so newly-arrived team-mates are noticed at once).
    if (observation._inventory._food < FOOD_RESERVE &&
        ownTile(observation).contains(ObjectType::Food)) {
        return "Take food";
    }
    return "";
}

std::string ScriptedPolicy::maybeBeacon()
{
    // The conductor beacons often so homing followers keep a fresh bearing;
    // everyone else chimes in rarely, just enough that a smaller id can surface
    // and take over should the current conductor die.
    const long period = isConductor() ? BEACON_FAST : BEACON_SLOW;
    if (_tick - _beaconTick < period) {
        return "";
    }
    _beaconTick = _tick;
    return "Broadcast Z " + std::to_string(_id);
}

std::string ScriptedPolicy::forageMove(const Observation &observation)
{
    const Tile &tile = ownTile(observation);
    if (tile.contains(_forageType)) {
        _foraging = false; // grabbed it - the homing leg will carry it back
        return "Take " + objectTypeToString(_forageType);
    }
    const int index = nearestTile(observation._vision, _forageType);
    if (index > 0) {
        return stepToward(index);
    }
    _foraging = false; // target slipped out of sight: resume homing next tick
    return homeToRally(observation);
}

std::string ScriptedPolicy::homeToRally(const Observation &observation)
{
    if (_foraging) {
        return forageMove(observation);
    }
    const Tile &tile = ownTile(observation);
    // Bank free food generously while travelling - enough to survive the trip and
    // to ferry a surplus to the rally buffer - but only detour for it when low.
    if (observation._inventory._food < FOOD_FERRY && tile.contains(ObjectType::Food)) {
        return "Take food";
    }
    if (observation._inventory._food < FOOD_LOW) {
        const int foodIndex = nearestTile(observation._vision, ObjectType::Food);
        if (foodIndex > 0) {
            return stepToward(foodIndex);
        }
    }
    // Free stone pickups en route stock the altar at no detour cost.
    for (const ObjectType stone : STONE_ORDER) {
        if (tile.contains(stone)) {
            return "Take " + objectTypeToString(stone);
        }
    }
    if (_conductorDir == 0) {
        _atRally = true; // co-located: stop and join the rally
        return holdAtRally(observation);
    }
    // Approach by sight whenever any cluster is visible - vision refreshes every
    // tick and lands us on an exact tile, which the coarse, stale 8-way bearing
    // cannot. First climb the density gradient (toward a more crowded tile)...
    const int crowd = mostCrowdedTile(observation._vision);
    if (crowd > 0) {
        return stepToward(crowd);
    }
    // ...and if no gradient exists yet (the conductor is still alone), lock onto
    // the nearest visible player by sight - this breaks the orbit-forever cycle
    // that a lone, gradient-less conductor used to trap homing drones in.
    if (ownTile(observation).count(ObjectType::Player) <= 1) {
        const int player = nearestPlayerTile(observation._vision);
        if (player > 0) {
            return stepToward(player);
        }
    }
    if (_conductorDir < 0) {
        return explore(); // no bearing yet: wander until a beacon is heard
    }
    return stepTowardSound(_conductorDir);
}

std::string ScriptedPolicy::runRally(const Observation &observation, int level)
{
    const int food = observation._inventory._food;
    const Tile &tile = ownTile(observation);
    const LevelRequirement &requirement = requirementFor(level);

    // Bank food first so the wait and the freeze that follow never starve us.
    if (food < FOOD_RESERVE && tile.contains(ObjectType::Food)) {
        return "Take food";
    }
    // Ferry the carried surplus onto the rally tile as a shared food buffer: the
    // never-moving conductor and the parked high-level cohort then eat in place,
    // which is what lets a quorum actually hold together long enough to incant.
    if (food > FOOD_RESERVE && tile.count(ObjectType::Food) < FOOD_BUFFER) {
        return "Set food";
    }
    // The rally tile turns into a food desert when crowded: a follower getting
    // low slips out to a nearby food tile and homes straight back. The conductor
    // anchors the centre, so it rides out the wait on respawns instead.
    if (food < FOOD_LOW && !isConductor()) {
        const int foodIndex = nearestTile(observation._vision, ObjectType::Food);
        if (foodIndex > 0) {
            _atRally = false;
            return stepToward(foodIndex);
        }
    }
    // Climb takes top priority: the instant the altar holds the stones and the
    // tile holds the quorum, fire. The server picks exactly the required players
    // and freezes them; a wrong-level/short ko is instant, so spamming is free.
    if (altarStocked(level, tile) && tile.count(ObjectType::Player) >= requirement._players &&
        food >= FOOD_INCANT) {
        return "Incantation";
    }
    // Stock the altar one stone at a time: drop only the specific stone the
    // ritual still lacks (never dump the whole inventory - spares feed later
    // levels and keep us from a pointless Set/Incant loop).
    const ObjectType need = missingStone(level, tile);
    if (need != ObjectType::Unknown) {
        if (observation._inventory.get(need) > 0) {
            return "Set " + objectTypeToString(need);
        }
        // Missing a stone we do not carry. The conductor never leaves (it anchors
        // the rally). A follower may make a short fetch trip - but most must stay
        // put or the quorum dissolves the moment it forms, so foraging is rationed:
        // anyone may leave while the quorum is not yet present (we cannot incant
        // anyway), but once it is, only a staggered ~1/4 slice fetches at a time
        // while the rest hold the tile. Trips are also kept short (nearby tiles).
        if (!isConductor()) {
            const int index = nearestNeighbour(observation._vision, need);
            const bool reachable = index > 0 && offsetForIndex(index)._forward <= FORAGE_RADIUS;
            const bool quorumPresent = tile.count(ObjectType::Player) >= requirement._players;
            const bool myTurn =
                ((static_cast<unsigned>(_id) ^ static_cast<unsigned>(_tick / FORAGE_STAGGER)) & 3u) == 0u;
            if (reachable && (!quorumPresent || myTurn)) {
                _foraging = true;
                _forageType = need;
                _atRally = false;
                return stepToward(index);
            }
        }
    }
    // Pool every spare stone onto the shared altar (incanting already had its
    // chance above, so this never loops). It stockpiles the rare stones -
    // mendiane, phiras, thystame - that the high-level rituals demand, long
    // before the cohort that needs them arrives.
    for (const ObjectType stone : STONE_ORDER) {
        if (observation._inventory.get(stone) > 0) {
            return "Set " + objectTypeToString(stone);
        }
    }
    return holdAtRally(observation);
}

std::string ScriptedPolicy::explore() const
{
    return (_tick % 4 == 0) ? "Right" : "Forward";
}

std::string ScriptedPolicy::decide(const Observation &observation)
{
    _tick++;
    const int level = observation._vision.empty() ? _level : inferLevel(observation._vision.size());
    _level = level;
    ingestBroadcasts(observation);

    const std::string command = commit(plan(observation, level));
    debug(observation, level, command);
    return command;
}

std::string ScriptedPolicy::commit(std::string command)
{
    // A bare turn rotates without closing distance, so two turns in a row would
    // oscillate a diagonal target in and out of the cone. Force a forward step
    // after every turn: turn to bear, then walk.
    const bool isTurn = command == "Left" || command == "Right";
    if (isTurn && _justTurned) {
        _justTurned = false;
        return "Forward";
    }
    _justTurned = isTurn;
    return command;
}

std::string ScriptedPolicy::plan(const Observation &observation, int level)
{
    const int food = observation._inventory._food;

    if (food <= FOOD_CRITICAL) {
        return seekFood(observation); // survival preempts every other goal
    }
    // Keep the beacon alive even while busy so the conductor stays locatable and
    // the election keeps converging. Survival above always wins.
    const std::string beacon = maybeBeacon();
    if (!beacon.empty()) {
        return beacon;
    }
    // Universal hunger guard: below the comfort line, eating preempts climbing in
    // every state (homing, waiting, foraging). Food is the most abundant resource
    // and respawns, so this is what keeps the cohort alive through the long waits
    // and the eating-blind incantation freezes.
    const std::string eat = maybeEat(observation, FOOD_LOW);
    if (!eat.empty()) {
        return eat;
    }
    if (level >= MAX_LEVEL) {
        return holdAtRally(observation); // maxed out: hold the tile and survive
    }
    // Fork at the rally: eggs hatch where they are laid, so replacements respawn
    // on the rally tile instead of scattered, tightening convergence over time.
    if (atRally() && _forks < MAX_FORKS && food >= FOOD_FORK && level >= 2) {
        _forks++;
        return "Fork";
    }
    if (atRally()) {
        return runRally(observation, level);
    }
    return homeToRally(observation);
}

void ScriptedPolicy::debug(const Observation &observation, int level, const std::string &command) const
{
    static const bool on = std::getenv("ZAPPY_DEBUG") != nullptr;
    if (!on) {
        return;
    }
    const char *role = isConductor() ? "cond" : (atRally() ? "rally" : (_foraging ? "forage" : "home"));
    std::cerr << "[#" << _id << " t" << _tick << "] L" << level
              << " food=" << observation._inventory._food << " " << role
              << " cond=" << _conductorId << " cdir=" << _conductorDir
              << " players0=" << ownTile(observation).count(ObjectType::Player)
              << " -> '" << (command.empty() ? "(wait)" : command) << "'\n";
}

} // namespace ai
