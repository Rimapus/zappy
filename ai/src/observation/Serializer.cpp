/*
** EPITECH PROJECT, 2026
** Serializer.cpp
** File description:
** Serialize an Observation to a one-line JSON string for the Python bridge
*/

#include "Serializer.hpp"

#include "parsing/Look.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace ai {

namespace {

/// Resources of the inventory, in the order they appear in the JSON object.
constexpr std::array<ObjectType, 7> INVENTORY_ORDER{
    ObjectType::Food,
    ObjectType::Linemate,
    ObjectType::Deraumere,
    ObjectType::Sibur,
    ObjectType::Mendiane,
    ObjectType::Phiras,
    ObjectType::Thystame,
};

/// Append @p text to @p out as a JSON string literal (quotes included),
/// escaping the characters required by the JSON spec.
void appendJsonString(std::ostringstream &out, const std::string &text)
{
    out << '"';
    for (const char character : text) {
        switch (character) {
        case '"':
            out << "\\\"";
            break;
        case '\\':
            out << "\\\\";
            break;
        case '\b':
            out << "\\b";
            break;
        case '\f':
            out << "\\f";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\r':
            out << "\\r";
            break;
        case '\t':
            out << "\\t";
            break;
        default:
            // Remaining control characters must be escaped as \u00XX.
            if (static_cast<unsigned char>(character) < 0x20) {
                out << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                    << static_cast<int>(static_cast<unsigned char>(character))
                    << std::dec << std::setfill(' ');
            } else {
                out << character;
            }
            break;
        }
    }
    out << '"';
}

/// Append the inventory object, e.g. {"food":10,"linemate":0,...}.
void appendInventory(std::ostringstream &out, const Inventory &inventory)
{
    out << '{';
    for (std::size_t i = 0; i < INVENTORY_ORDER.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        const ObjectType type = INVENTORY_ORDER[i];
        out << '"' << objectTypeToString(type) << "\":" << inventory.get(type);
    }
    out << '}';
}

/// Append a single tile as a JSON array of object names, e.g. ["food","linemate"].
void appendTile(std::ostringstream &out, const Tile &tile)
{
    out << '[';
    for (std::size_t i = 0; i < tile._objects.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        out << '"' << objectTypeToString(tile._objects[i]) << '"';
    }
    out << ']';
}

/// Append the vision as a JSON array of tiles.
void appendVision(std::ostringstream &out, const std::vector<Tile> &vision)
{
    out << '[';
    for (std::size_t i = 0; i < vision.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        appendTile(out, vision[i]);
    }
    out << ']';
}

/// Append the broadcasts as a JSON array of {"direction":K,"message":"..."}.
void appendBroadcasts(std::ostringstream &out, const std::vector<Broadcast> &broadcasts)
{
    out << '[';
    for (std::size_t i = 0; i < broadcasts.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        out << "{\"direction\":" << broadcasts[i]._direction << ",\"message\":";
        appendJsonString(out, broadcasts[i]._message);
        out << '}';
    }
    out << ']';
}

} // namespace

std::string serialize(const Observation &observation)
{
    std::ostringstream out;

    out << "{\"level\":" << observation._level << ",\"inventory\":";
    appendInventory(out, observation._inventory);
    out << ",\"vision\":";
    appendVision(out, observation._vision);
    out << ",\"broadcasts\":";
    appendBroadcasts(out, observation._broadcasts);
    out << ",\"free_slots\":" << observation._freeSlots << "}\n";

    return out.str();
}

} // namespace ai
