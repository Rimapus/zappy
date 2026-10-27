/*
** EPITECH PROJECT, 2026
** Serializer.hpp
** File description:
** Serialize an Observation to a one-line JSON string for the Python bridge
*/

#pragma once

#include "observation/Observation.hpp"

#include <string>

namespace ai {

/**
 * @brief Serialize an Observation into a single JSON line for the Python bridge.
 *
 * The whole observation is flattened to one compact JSON object terminated by a
 * single '\n', so the Python side can read it with a plain line-based protocol
 * (one `readline()` == one observation). The layout mirrors the Observation
 * struct field by field:
 * @code
 * {"level":1,"inventory":{"food":10,"linemate":0,"deraumere":0,"sibur":0,
 *  "mendiane":0,"phiras":0,"thystame":0},"vision":[["player"],["food","linemate"],[]],
 *  "broadcasts":[{"direction":3,"message":"hello"}],"free_slots":2}
 * @endcode
 *
 * Each vision tile is the ordered list of object names it holds (duplicates
 * kept), reusing objectTypeToString(); empty tiles yield `[]`. Broadcast
 * messages are JSON-escaped, so any character (quotes, backslashes, control
 * characters) is transmitted safely.
 *
 * @param observation  Snapshot of the game state to serialize.
 * @return A single line of JSON, ending with '\n'.
 */
std::string serialize(const Observation &observation);

} // namespace ai
